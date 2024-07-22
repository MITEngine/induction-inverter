import math
import struct
import time
import tkinter as tk

from PIL import Image, ImageTk

from serial_manager import SerialManager

manager = SerialManager("STM32 STLink", startswith=True)
angle = 0

data = bytearray([1])
data.extend(struct.pack("f", 200))
manager.write_bytes(data)
time.sleep(0.01)

class RotatingKnob(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Infinitely Rotating Knob")
        self.geometry("400x400")
        
        self.canvas = tk.Canvas(self, width=400, height=400)
        self.canvas.pack()

        self.original_knob_image = Image.open("knob.png")  # Load your knob image here
        self.knob_image = ImageTk.PhotoImage(self.original_knob_image)
        self.knob = self.canvas.create_image(200, 200, image=self.knob_image)
        
        self.angle = 0
        self.canvas.bind("<B1-Motion>", self.rotate_knob)

    def rotate_knob(self, event):
        dx = event.x - 200
        dy = event.y - 200
        new_angle = math.degrees(math.atan2(dy, dx))

        if not hasattr(self, 'previous_angle'):
            self.previous_angle = new_angle

        delta_angle = new_angle - self.previous_angle
        self.previous_angle = new_angle
        
        self.angle += delta_angle
        self.angle %= 360
        
        self.rotated_knob_image = self.rotate_image(self.original_knob_image, -75-self.angle)
        self.knob_image = ImageTk.PhotoImage(self.rotated_knob_image)
        
        self.canvas.delete(self.knob)
        self.knob = self.canvas.create_image(200, 200, image=self.knob_image)

        data = bytearray([0])
        data.extend(struct.pack("f", 7*3.1415*2*self.angle/360))
        manager.write_bytes(data)
        time.sleep(0.001)

    def rotate_image(self, image, angle):
        return image.rotate(angle, expand=True)

if __name__ == "__main__":
    app = RotatingKnob()
    app.mainloop()
