from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from flask_cors import CORS
import os
import threading
import paho.mqtt.client as mqtt
from datetime import datetime

app = Flask(__name__)

app.secret_key = os.urandom(24)

CORS(app)

app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///database.db"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

db = SQLAlchemy(app)

class Database(db.Model):
    __tablename__ = 'NODEMCU'

    id = db.Column(db.Integer, primary_key=True)
    date = db.Column(db.String(10), nullable=False)
    temperatura = db.Column(db.String(255), nullable=False)

    def __init__(self, temperatura):
        self.date = datetime.now().strftime("%Y-%m-%d")
        self.temperatura = temperatura

    def __repr__(self):
        return f"{self.date} - {self.temperatura}"

migrate = Migrate(app, db)

def save_temperature(temperatura):
    try:
        new_entry = Database(temperatura=temperatura)
        db.session.add(new_entry)
        db.session.commit()
    except Exception as e:
        db.session.rollback()

def on_connect(client, userdata, flags, rc):
    client.subscribe("cesul_prova_de_iot/temperatura")

def on_message(client, userdata, msg):
    temperatura = msg.payload.decode()
    
    with app.app_context():
        save_temperature(temperatura)

def mqtt_thread():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("test.mosquitto.org", 1883, 60)
    client.loop_forever()

if __name__ == '__main__':
    threading.Thread(target=mqtt_thread, daemon=True).start()
    app.run(debug=True)
