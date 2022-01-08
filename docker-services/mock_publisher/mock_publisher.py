import time
import random
import json
import paho.mqtt.client as paho

broker="127.0.0.1"
port=1883
user = "testuser"
pswd = "123456"
topic = "home/power"


def on_publish(client, userdata, rc):
    print(f"on_publish; rc={rc}")

def on_connect(client, userdata, flags, rc):
    print(f"on_connect; rc={rc}")
    if rc==0:
        client.connected_flag=True #set flag
    else:
        print("Bad connection Returned code=",rc)
        client.bad_connection_flag=True


def get_measure():
    v = 220
    c = random.random() * 10
    p = random.random() * 100
    e = random.random()
    f = 50
    pf = 0.03
    return {"voltage": v, "current": c, "power": p, "energy": e, "frequency": f, "pf": pf}


if __name__ == "__main__":
    client = paho.Client("mock_publisher")
    client.on_connect = on_connect
    client.on_publish = on_publish
    client.username_pw_set(username=user,password=pswd)

    connected = False
    while not connected:
        try:
            print("Connecting...")
            rc = client.connect(broker, port)
            connected = True
        except Exception as e:
            print(f"Exception {e}\nRetrying in 5 seconds...")
            time.sleep(5)

    while True:
        message = json.dumps(get_measure())
        ret= client.publish(topic, message)
        time.sleep(5)
