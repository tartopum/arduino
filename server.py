from collections import defaultdict
import json
import os
from threading import Thread, Lock
import time

from flask import Flask, render_template, redirect, request, url_for, jsonify
import plotly
from plotly.graph_objs import Scatter, Layout
from serial import Serial

app = Flask(__name__)
app.debug = True

exp_name = None
measures = defaultdict(list)
error_msg = ""
lock = Lock()

try:
    os.makedirs("data")
except FileExistsError:
    pass


def read_serial(lock):
    with Serial("/dev/ttyACM0", 9600, timeout=1) as ser:
        while True:
            time.sleep(1)
            if exp_name is None:
                continue

            line = ser.readline().decode("utf-8")
            try:
                data = json.loads(line)
            except (json.decoder.JSONDecodeError, UnicodeDecodeError):
                pass
            else:
                with lock:
                    for k, v in data.items():
                        measures[k].append(v)


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


def mask_array(a, mask, filter_value=1):
    return [v for i, v in enumerate(a) if mask[i] == filter_value]


def get_plot(pressure1, pressure2, arm_pos, exp_name, date):
    ARM_IN_POS_COLOR = "rgb(119,221,119)"
    ARM_NOT_IN_POS_COLOR = "rgb(255,105,97)"

    x = range(len(pressure1))
    x_arm_in_pos = mask_array(x, arm_pos)
    x_arm_not_in_pos = mask_array(x, arm_pos, 0)

    fig = plotly.tools.make_subplots(rows=2, cols=1, shared_xaxes=True, vertical_spacing=0.05)

    for arm_in_pos, color in enumerate((ARM_NOT_IN_POS_COLOR, ARM_IN_POS_COLOR)):
        name = "Bras en position" if arm_in_pos else "Bras ailleurs"
        fig.append_trace(
            Scatter(
                x=mask_array(x, arm_pos, arm_in_pos),
                y=mask_array(pressure1, arm_pos, arm_in_pos),
                mode="markers",
                name=name,
                legendgroup=arm_in_pos,
                hoverinfo="x+y",
                marker=dict(
                    color=color,
                ),
            ),
            1, 1
        )
        fig.append_trace(
            Scatter(
                x=mask_array(x, arm_pos, arm_in_pos),
                y=mask_array(pressure2, arm_pos, arm_in_pos),
                mode="markers",
                name=name,
                legendgroup=arm_in_pos,
                showlegend=False,
                hoverinfo="x+y",
                marker=dict(
                    color=color,
                ),
            ),
            2, 1
        )

    fig["layout"].update(title=f"{exp_name} ({date})", width=800, height=400, margin=dict(t=100))
    fig["layout"]["xaxis1"].update(title="Temps", range=[-1, len(pressure1)], showline=True, zeroline=False,)
    fig["layout"]["yaxis1"].update(title="P1", showline=True)
    fig["layout"]["yaxis2"].update(title="P2", showline=True)

    return plotly.offline.plot(fig, include_plotlyjs=False, output_type="div", show_link=False,)


def list_exps():
    exps = []
    for name in os.listdir("data"):
        path = os.path.join("data", name)
        date = time.strftime(
            "%Y-%m-%d %H:%M:%S",
            time.gmtime(os.path.getmtime(path))
        )
        try:
            with open(path) as f:
                data = json.load(f)
            p1 = data["pressure1"]
            p2 = data["pressure2"]
            arm_pos = data["arm_pos"]
        except (ValueError, json.decoder.JSONDecodeError, KeyError):
            continue
        plot = get_plot(p1, p2, arm_pos, name, date)
        exps.append((date, name, plot))
    return reversed(sorted(exps))


@app.route("/")
def home():
    global error_msg
    err = error_msg
    error_msg = ""
    if exp_name is None:
        exps = list_exps()
        return render_template("start_exp.html", exps=exps, err=err)
    plot = get_plot(measures["pressure1"], measures["pressure2"], measures["arm_pos"], exp_name, "")
    return render_template("current_exp.html", plot=plot, err=err)


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

    with lock:
        measures = defaultdict(list)
    exp_name = name
    return redirect(url_for("home"))


@app.route("/exp/stop", methods=["POST"])
def stop_exp():
    save_measures() 
    global exp_name
    exp_name = None
    return redirect(url_for("home"))


if __name__ == "__main__":
    thread = Thread(target=read_serial, args=(lock,))
    thread.start() 
    app.run()
    thread.join()
