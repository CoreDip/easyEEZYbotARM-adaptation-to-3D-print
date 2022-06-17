print("░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░")
print("░░░██████╗░░░░░█████╗░░█████╗░██████╗░███████╗  ░█████╗░██╗░░░██╗░██████╗░░░")
print("░░██╔════╝░░░░██╔══██╗██╔══██╗██╔══██╗██╔════╝  ██╔══██╗██║░░░██║██╔════╝░░░")
print("░░██║░░██╗░██░██║░░╚═╝██║░░██║██║░░██║█████╗░░  ███████║╚██╗░██╔╝██║░░██╗░░░")
print("░░██║░░╚██╗░░░██║░░██╗██║░░██║██║░░██║██╔══╝░░  ██╔══██║░╚████╔╝░██║░░╚██╗░░")
print("░░╚██████╔╝░░░╚█████╔╝╚█████╔╝██████╔╝███████╗  ██║░░██║░░╚██╔╝░░╚██████╔╝░░")
print("░░░╚═════╝░░░░░╚════╝░░╚════╝░╚═════╝░╚══════╝  ╚═╝░░╚═╝░░░╚═╝░░░░╚═════╝░░░")
print("░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░")
print()
print("                         █▀▀▄ █──█    █▀▀▄ ─▀─ █▀▀█ █▀▀ █▀▀█ █▀▀█ █▀▀ ")
print("                         █▀▀▄ █▄▄█    █──█ ▀█▀ █──█ █── █──█ █▄▄▀ █▀▀ ")
print("                         ▀▀▀─ ▄▄▄█    ▀▀▀─ ▀▀▀ █▀▀▀ ▀▀▀ ▀▀▀▀ ▀─▀▀ ▀▀▀")
print()


from easyEEZYbotARM.kinematic_model import EEZYbotARM_Mk2
from easyEEZYbotARM.serial_communication import arduinoController

import easygui


myArduino = arduinoController(port="COM8")

myArduino.openSerialPort()
myVirtualRobotArm = EEZYbotARM_Mk2(
    initial_q1=0, initial_q2=90, initial_q3=-130)

servoAngle_EE_closed = 10
servoAngle_EE_open = 90



with open(easygui.fileopenbox()) as file: #відкриваємо gcode файл
    array = [row.strip() for row in file] #записуємо кожну строку файлу в новий елемент масиву
    mass = []
    num=0
    for element in array:
        if array[num-1][0:2] == "G1":
            test=array[num-1]
            test1=test.split()[1:]
            if len(test1) ==3: #якщо в рядку 3шт координати (X, Y, E)
                mass.append({'X': test1[0][1:], 'Y': test1[1][1:], 'E': test1[2][1:]})
        num=num+1

    lic=0
    totalX = 0
    totalY = 0
    totalE = 0
    for element in mass:
        totalX=totalX+float(mass[lic]['X'])
        totalY=totalY+float(mass[lic]['Y'])
        totalE=totalE+float(mass[lic]['E'])
        lic=lic+1
    AvgX=totalX/lic #Середнє значення всіх Х
    AvgY=totalY/lic #Середнє значення всіх Y
    AvgE=totalE/lic #Середнє значення всіх E
    print('X: ', int(AvgX), ' Y: ', int(AvgY),  ' E: ', int(AvgE))


x= int(AvgX)+150 #Калібровка координат(залежить від розміщення РП відносно столу)
y= int(AvgY)-96 #Калібровка координат(залежить від розміщення РП відносно столу)
z= 70 #висота

a1, a2, a3 = myVirtualRobotArm.inverseKinematics(x, y, z) #перетворюємо координатии в кути для сервоприводів

myVirtualRobotArm.updateJointAngles(q1=a1, q2=a2, q3=a3)
servoAngle_q1, servoAngle_q2, servoAngle_q3 = myVirtualRobotArm.map_kinematicsToServoAngles()

myArduino.communicate(data=myArduino.composeMessage(servoAngle_q1=90, #поворот до столу
                                                    servoAngle_q2=90,
                                                    servoAngle_q3=60,
                                                    servoAngle_EE=servoAngle_EE_closed))

myArduino.communicate(data=myArduino.composeMessage(servoAngle_q1=servoAngle_q1, #виконання "зкидання"
                                                    servoAngle_q2=servoAngle_q2,
                                                    servoAngle_q3=servoAngle_q3,
                                                    servoAngle_EE=servoAngle_EE_open))

myArduino.communicate(data=myArduino.composeMessage(servoAngle_q1=90, #повернення до основи
                                                    servoAngle_q2=90,
                                                    servoAngle_q3=60,
                                                    servoAngle_EE=servoAngle_EE_closed))

myArduino.communicate(data=myArduino.composeMessage(servoAngle_q1=0, #поворот в початкове положення
                                                    servoAngle_q2=90,
                                                    servoAngle_q3=60,
                                                    servoAngle_EE=servoAngle_EE_closed))


myArduino.closeSerialPort()

input('Натисніть Enter щоб вийти')