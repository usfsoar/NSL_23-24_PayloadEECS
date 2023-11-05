from Server.Config import *

@app.route('/hello')
def index():
    return "Hello world!"