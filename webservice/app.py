import os
import hashlib
import time
import zipfile
import subprocess
import sys
from pathlib import Path
from flask import Flask, render_template, send_file, request, redirect, session

app = Flask(__name__)
app.secret_key = "supersecret_key"

CHALLENGE_TIME = 60
flag = "vulnerable_function"



def get_time():
    starting_time = session.get("start_time")
    if not starting_time:
        return None

    elapsed = time.time() - starting_time
    return max(0, int(CHALLENGE_TIME - elapsed))


def build_challenge_name(output_dir):
    if not output_dir.exists() or not output_dir.is_dir():
        return "challenge"

    output_files = sorted(
        file_path
        for file_path in output_dir.iterdir()
        if file_path.is_file() and file_path.name != "Makefile"
    )
    if not output_files:
        return "challenge"

    first_output_file = output_files[0]
    with open(first_output_file, "rb") as file_data:
        digest = hashlib.sha256(file_data.read()).hexdigest()

    return f"challenge_{digest[:5]}"


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
    session["solved"] = False

    # Run obfuscator each time to generate a fresh obfuscation
    base_dir = Path(__file__).resolve().parent.parent
    obfuscator_script = base_dir / "obfuscator" / "obfuscator.py"
    input_dir = base_dir / "configloader"
    output_dir = base_dir / "obfuscator" / "out"

    # Ensure tmp exists inside webservice
    tmp_dir = base_dir / "webservice" / "tmp"
    tmp_dir.mkdir(parents=True, exist_ok=True)
    zip_path = tmp_dir / "challenge.zip"

    try:
        # Call the obfuscator as a subprocess so it produces a new, randomized output
        subprocess.run(
            [sys.executable, str(obfuscator_script), "-i", str(input_dir), "-o", str(output_dir)],
            check=True,
            cwd=str(base_dir),
        )
    except subprocess.CalledProcessError:
        return render_template(
            "index.html",
            result="Obfuscation failed. Try again.",
            result_type="error",
            remaining_time=get_time(),
            solved=False,
        )

    session["start_time"] = time.time()

    # Package the obfuscated output files into a zip and return
    challenge_name = build_challenge_name(output_dir)
    zip_path = tmp_dir / f"{challenge_name}.zip"

    with zipfile.ZipFile(str(zip_path), "w") as f_zip:
        if output_dir.exists() and output_dir.is_dir():
            for file_name in sorted(os.listdir(str(output_dir))):
                file_path = output_dir / file_name
                if file_path.is_file():
                    f_zip.write(str(file_path), arcname=file_name)

    return send_file(str(zip_path), as_attachment=True, download_name=f"{challenge_name}.zip")


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
    app.run(host="0.0.0.0", debug=os.getenv("FLASK_DEBUG", "0") == "1")