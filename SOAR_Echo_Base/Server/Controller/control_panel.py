from Server.Config import *
from flask import Flask, render_template

@app.route('/')
def control_panel():
    return render_template('control_panel.html')