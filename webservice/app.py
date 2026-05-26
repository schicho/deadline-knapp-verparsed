import os
import hashlib
import time
import zipfile
import subprocess
import sys
import shutil
import secrets
from pathlib import Path
from flask import Flask, render_template, send_file, request, redirect, session
import json

app = Flask(__name__)
app.secret_key = "supersecret_key"


CHALLENGE_TIME = 60
flag = "vulnerable_function"

# When set to "1", keep per-run artifacts for debugging instead of deleting them.
KEEP_RUNS = os.getenv("FLASK_DEBUG", "0") == "1"

def get_time():
    starting_time = session.get("start_time")
    if not starting_time:
        return None

    elapsed = time.time() - starting_time
    return max(0, int(CHALLENGE_TIME - elapsed))


def is_submission_locked(remaining_time=None):
    if remaining_time is None:
        remaining_time = get_time()

    return (
        session.get("solved", False)
        or session.get("attempt_used", False)
        or (remaining_time is not None and remaining_time <= 0)
    )


def build_challenge_name(run_id):
    return f"challenge_{run_id}"


def generate_run_id():
    return f"{secrets.randbelow(10 ** 8):08d}"


def load_output_dict(obfuscator_dir):
    output_dict_path = obfuscator_dir / "output_dict.json"

    if not output_dict_path.is_file():
        raise FileNotFoundError(f"Missing obfuscation metadata: {output_dict_path}")

    with open(output_dict_path, "r", encoding="utf-8") as file_data:
        return json.load(file_data)


def cleanup_run_artifacts(run_dir):
    if not KEEP_RUNS:
        print(f"Cleaning up run artifacts: {run_dir}")
        shutil.rmtree(run_dir, ignore_errors=True)
    else:
        print(f"Keeping run artifacts for debugging: {run_dir}")


@app.route("/")
def index():
    remaining_time = get_time()
    return render_template(
        "index.html",
        result=None,
        result_type=None,
        remaining_time=remaining_time,
        solved=session.get("solved", False),
        submission_locked=is_submission_locked(remaining_time)
    )


@app.route("/start")
def start():
    session.clear()
    session["solved"] = False

    # Run obfuscator each time to generate a fresh obfuscation
    base_dir = Path(__file__).resolve().parent.parent
    obfuscator_script = base_dir / "obfuscator" / "obfuscator.py"
    input_dir = base_dir / "configloader"

    # Ensure tmp exists inside webservice
    tmp_dir = base_dir / "webservice" / "tmp"
    tmp_dir.mkdir(parents=True, exist_ok=True)
    run_id = generate_run_id()
    run_dir = tmp_dir / run_id
    output_dir = run_dir / "out"
    zip_name = f"{build_challenge_name(run_id)}.zip"
    zip_path = run_dir / zip_name

    run_dir.mkdir(parents=True, exist_ok=False)

    try:
        # Call the obfuscator as a subprocess so it produces a new, randomized output
        subprocess.run(
            [sys.executable, str(obfuscator_script), "-i", str(input_dir), "-o", str(output_dir)],
            check=True,
            cwd=str(base_dir),
        )
    except subprocess.CalledProcessError:
        cleanup_run_artifacts(run_dir)
        return render_template(
            "index.html",
            result="Obfuscation failed. Try again.",
            result_type="error",
            remaining_time=get_time(),
            solved=False,
            submission_locked=False,
        )
    
    # load the output dict to get the correct answer for the current obfuscation
    try:
        output_dict = load_output_dict(output_dir)
    except (FileNotFoundError, json.JSONDecodeError):
        cleanup_run_artifacts(run_dir)
        return render_template(
            "index.html",
            result="Failed to load obfuscation metadata.",
            result_type="error",
            remaining_time=get_time(),
            solved=False,
            submission_locked=False,
        )

    session["answer"] = output_dict.get("flag", flag)
    session["flag_file"] = output_dict.get("flag_file")

    print(f"Generated challenge with answer: {session['answer']} (file: {session['flag_file']})")
    session["start_time"] = time.time()

    with zipfile.ZipFile(str(zip_path), "w") as f_zip:
        if output_dir.exists() and output_dir.is_dir():
            for file_name in sorted(os.listdir(str(output_dir))):
                if file_name == "output_dict.json":
                    continue
                file_path = output_dir / file_name
                if file_path.is_file():
                    f_zip.write(str(file_path), arcname=file_name)

    response = send_file(str(zip_path), as_attachment=True, download_name=zip_name)
    cleanup_run_artifacts(run_dir)

    return response


@app.route("/submit", methods=["POST"])
def submit():
    if "start_time" not in session:
        return render_template(
            "index.html",
            result="Start a challenge first!",
            result_type="warning",
            remaining_time=None,
            solved=False,
            submission_locked=False
        )

    remaining_time = get_time()

    if session.get("solved"):
        return render_template(
            "index.html",
            result="Correct!",
            result_type="success",
            remaining_time=remaining_time,
            solved=True,
            submission_locked=True
        )

    if remaining_time <= 0:
        return render_template(
            "index.html",
            result="Time is up!",
            result_type="warning",
            remaining_time=0,
            solved=False,
            submission_locked=True
        )

    if session.get("attempt_used", False):
        return render_template(
            "index.html",
            result="This challenge is already locked. Reset to try again.",
            result_type="warning",
            remaining_time=remaining_time,
            solved=False,
            submission_locked=True
        )

    session["attempt_used"] = True
    answer = request.form.get("answer", "").strip()

    if answer == session.get("answer"):
        message = "Correct!"
        message_type = "success"
        session["solved"] = True
    else:
        message = "Wrong answer. This challenge is now locked until reset."
        message_type = "error"

    return render_template(
        "index.html",
        result=message,
        result_type=message_type,
        remaining_time=remaining_time,
        solved=session.get("solved", False),
        submission_locked=is_submission_locked(remaining_time)
    )


@app.route("/reset")
def reset():
    session.clear()
    return redirect("/")


if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=os.getenv("FLASK_DEBUG", "0") == "1")