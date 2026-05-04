import os
import time
import zipfile
from flask import Flask, render_template, send_file, request, redirect, session, jsonify

app = Flask(__name__)
app.secret_key = "supersecret_key"

CHALLENGE_TIME = 60
flag = "vulnerable_function"

C_CODE = """
#include <stdio.h>
#include <stdlib.h>

void vulnerable_function(char *cmd) {
    system(cmd);
}

int main() {
    char command[256];
    fgets(command, sizeof(command), stdin);
    vulnerable_function(command);
    return 0;
}
"""


def get_time():
    starting_time = session.get("start_time")
    if not starting_time:
        return None

    elapsed = time.time() - starting_time
    return max(0, int(CHALLENGE_TIME - elapsed))


@app.route("/")
def index():
    return render_template(
        "index.html",
        result=None,
        result_type=None,
        remaining_time=get_time(),
        solved=session.get("solved", False)
    )


@app.route("/start")
def start():
    session.clear()
    session["answer"] = flag
    session["start_time"] = time.time()
    session["solved"] = False

    os.makedirs("tmp", exist_ok=True)
    zip_path = os.path.join("tmp", "challenge.zip")
    with open("tmp/challenge.c", "w") as f:
        f.write(C_CODE)

    with zipfile.ZipFile("tmp/challenge.zip", "w") as f_zip:
        f_zip.write("tmp/challenge.c", arcname="challenge.c")

    return send_file("tmp/challenge.zip", as_attachment=True)


@app.route("/time")
def remaining_time():
    remaining = get_time()

    return jsonify({
        "active": remaining is not None,
        "remaining_time": remaining,
        "solved": session.get("solved", False)
    })


@app.route("/submit", methods=["POST"])
def submit():
    if "start_time" not in session:
        return render_template(
            "index.html",
            result="Start a challenge first!",
            result_type="warning",
            remaining_time=None,
            solved=False
        )

    if session.get("solved"):
        return render_template(
            "index.html",
            result="Correct!",
            result_type="success",
            remaining_time=get_time(),
            solved=True
        )

    remaining_time = get_time()

    if remaining_time <= 0:
        return render_template(
            "index.html",
            result="Time is up!",
            result_type="warning",
            remaining_time=0,
            solved=False
        )

    answer = request.form.get("answer", "").strip()

    if answer == session.get("answer"):
        message = "Correct!"
        message_type = "success"
        session["solved"] = True
    else:
        message = "Wrong, try again!"
        message_type = "error"

    return render_template(
        "index.html",
        result=message,
        result_type=message_type,
        remaining_time=remaining_time,
        solved=session.get("solved", False)
    )


@app.route("/reset")
def reset():
    session.clear()
    return redirect("/")


if __name__ == "__main__":
    app.run(debug=True)

