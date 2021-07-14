#!/usr/bin/env python
import rospy 
from std_msgs.msg import Bool
from geometry_msgs.msg import Point
from gripper_node.srv import *
from ws4py.client.threadedclient import WebSocketClient
import json
import time 

class BasicClient(WebSocketClient):
    def opened(self):
        self.operation_mode = None
        self.responded = True

        privilege_request = ['request-privilege', 
                                {'privilege' : 'change-action-command',
                                 'priority' : 0}]
        self.send(json.dumps(privilege_request))


    def closed(self, code, reason):
        print(("Closed", code, reason))


    def received_message(self, m):
        dataloaded = json.loads(m.data)
        message_type = str(dataloaded[0])
        message_dict = dataloaded[1]

        if message_type == 'privileges':
            self.done = message_dict['privileges'][0]['has']
            if self.done:
                print("Privilege request executed successfully.")

        elif message_type == 'robot-status':
            self.responded = True
            self.operation_mode = str(message_dict['operation-mode'])

        elif message_type == 'error':
            self.error_info = str(message_dict['info'])
            print('Error: ', self.error_info)

        elif message_type == 'action-status-changed':
            if message_dict['status'] == 'running':
                # print('Command received and is running')
                self.completed = False

            elif message_dict['status'] == 'success':
                # print('Command finished successfully executing. ', str(message_dict['info']))
                self.completed = True


class Gripper_node:
    def __init__(self):
        rospy.init_node("gripper_node")
        self.left_gripper_pub = rospy.Publisher("/left_gripper", Bool, queue_size=1)
        self.right_gripper_pub = rospy.Publisher("/right_gripper", Bool, queue_size=1)


    def open_gripper(self, gripper_name):
        msg = Bool()
        msg.data = True
        if gripper_name == "left":
            self.left_gripper_pub.publish(msg)

        else:
            self.right_gripper_pub.publish(msg)


    def close_gripper(self, gripper_name):
        msg = Bool()
        msg.data = False
        if gripper_name == "left":
            self.left_gripper_pub.publish(msg)

        else:
            self.right_gripper_pub.publish(msg)


class Arm_Conf_node:
    def __init__(self):
        rospy.init_node("arm_conf_node")
        self.left_arm_pub = rospy.Publisher("/left_arm_conf", Point, queue_size=1)

    def send_conf_values(self, values):
        try:
            channel = rospy.ServiceProxy("conf_channel", Conf)
            send_request = ConfRequest()

            send_request.conf.x = values[0]
            send_request.conf.y = values[1]
            send_request.conf.z = values[2]

            response = channel(send_request)
            if response.status:
               print("successfully sent conf ",values) 
            else:
                print("failed to send conf ",values)

        except rospy.ServiceException as e:
            print(e)
            sys.exit()



def send_message(ws, message):
    ws.send(json.dumps(message))


def pick(ws):
    pre_pick_msg = ["action-end-effector-move",
      {
        "end-effector": "right-hand",
        "waypoints": [
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,-0.25,0.15]},
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,-0.15,0.25]}],
        "reference-frame": {
          "robot-frame": "base"
        },
        "stall-threshold": None,
        "cyclic": False,
        "max-speed": 0.5,
        "duration": 2.5,
        "transition-duration": None
      }, 0]

    pick_msg = ["action-end-effector-move",
      {
        "end-effector": "right-hand",
        "waypoints": [
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,-0.25,0.15]},
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,-0.25,0.15]}],
        "reference-frame": {
          "robot-frame": "base"
        },
        "stall-threshold": None,
        "cyclic": False,
        "max-speed": 0.5,
        "duration": 2.5,
        "transition-duration": None
      }, 0]

    print('sending pre-grasp pose')
    # send_message(ws, pre_pick_msg)
    # time.sleep(5)

    print('sending pick pose')
    send_message(ws, pick_msg)
    time.sleep(5)


def place(ws):
    pre_place_msg = ["action-end-effector-move",
      {
        "end-effector": "right-hand",
        "waypoints": [
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,0.05,0.25]},
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,0.05,0.25]}],
        "reference-frame": {
          "robot-frame": "base"
        },
        "stall-threshold": None,
        "cyclic": False,
        "max-speed": 0.5,
        "duration": 2.5,
        "transition-duration": None
      }, 0]


    place_msg = ["action-end-effector-move",
      {
        "end-effector": "right-hand",
        "waypoints": [
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,0.05,0.15]},
                      {"rpyxyz":[-0.0136,0.8128,0.1109,0.4,0.05,0.15]}],
        "reference-frame": {
          "robot-frame": "base"
        },
        "stall-threshold": None,
        "cyclic": False,
        "max-speed": 0.5,
        "duration": 2.5,
        "transition-duration": None
      }, 0]


    print('sending pre-place pose')
    # send_message(ws, pre_place_msg)
    # time.sleep(10)

    print('sending place pose')
    send_message(ws, place_msg)
    time.sleep(5)








if __name__ == "__main__":
    conf = Arm_Conf_node()
    for i in range(10):
        values = [999,90,90]
        conf.send_conf_values(values)
        print("sent 90")
        time.sleep(5)
        values = [10,0,0]
        conf.send_conf_values(values)
        print("sent 0")
        time.sleep(5)

    '''
    gripper = Gripper_node()
    for i in range(1000):
        gripper.open_gripper('left')
        time.sleep(2)
        gripper.close_gripper('left')
        time.sleep(2)
    '''


'''
    gripper = Gripper_node()
    ws = BasicClient('ws://10.10.2.1:8080', protocols=['json-v1-agility'])
    ws.daemon = False

    while True:
        try:
            ws.connect()
            print("WS connection established")
            time.sleep(1)
            break
        except:
            print('WS connection NOT established')
            time.sleep(1)


    for i in range(10):
        print('Starting grasping demo')

        print('Picking...')
        gripper.open_gripper('left')
        time.sleep(5)
        pick(ws)
        time.sleep(5)
        gripper.close_gripper('left')
        time.sleep(5)


        print('Placing...')
        place(ws)
        time.sleep(5)
        gripper.open_gripper('left')
        time.sleep(5)

        print('Demo completed')

'''

