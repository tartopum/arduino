import json
import os
import time

from flask import Flask, render_template, redirect, request, url_for, jsonify
app = Flask(__name__)
app.debug = True

exp_name = None
measures = []
error_msg = ""

try:
    os.makedirs("data")
except FileExistsError:
    pass


def get_path(name):
    return os.path.join("data", f"{name}.json")


def is_old_exp(name):
    return os.path.isfile(get_path(name))


def save_measures():
    global exp_name
    if exp_name is None:
        return
    with open(get_path(exp_name), "w") as f:
        json.dump(measures, f)


def list_exps():
    exps = []
    for name in os.listdir("data"):
        try:
            path = os.path.join("data", name)
            with open(path) as f:
                t = time.gmtime(os.path.getmtime(path))
                exps.append((time.strftime("%Y-%m-%d %H:%M:%S", t), name, json.load(f)))
        except (ValueError, json.JSONDecodeError):
            pass
    return reversed(sorted(exps))


@app.route("/")
def home():
    global error_msg
    err = error_msg
    error_msg = ""
    if exp_name is None:
        return render_template("start_exp.html", exps=list_exps(), err=err)
    return render_template("current_exp.html", exp_name=exp_name, measures=measures, err=err)


@app.route("/exp/start", methods=["POST"])
def start_exp():
    global exp_name
    global measures
    global error_msg
    name = request.form["name"] 
    if not name:
        error_msg = "Le nom ne peut être vide."
        return redirect(url_for("home"))
    if is_old_exp(name):
        error_msg = f"Le fichier '{name}' existe déjà."
        return redirect(url_for("home"))
    measures = []
    exp_name = name
    return redirect(url_for("home"))


@app.route("/exp/stop", methods=["POST"])
def stop_exp():
    save_measures() 
    global exp_name
    exp_name = None
    return redirect(url_for("home"))


@app.route("/measures", methods=["GET", "POST"])
def measures_():
    if request.method == "GET":
        return jsonify(measures)
    if exp_name is not None:
        measures.extend(request.get_json())
    return ""


if __name__ == "__main__":
    app.run()
