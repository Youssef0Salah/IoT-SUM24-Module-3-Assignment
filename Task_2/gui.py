import tkinter as tk
import requests

ESP32_IP_ADDRESS = "192.168.204.169"  # Replace with the actual IP address of your ESP32

class LedControlApp:
    def __init__(self, root):
        self.root = root
        self.root.title("ESP32 LED Control")

        self.button_on = tk.Button(root, text="Turn LED On", font=("Helvetica", 16), command=self.turn_led_on)
        self.button_on.pack(pady=10)

        self.button_off = tk.Button(root, text="Turn LED Off", font=("Helvetica", 16), command=self.turn_led_off)
        self.button_off.pack(pady=10)

        self.label_status = tk.Label(root, text="LED Status: Unknown", font=("Helvetica", 16))
        self.label_status.pack(pady=10)

    def turn_led_on(self):
        response = requests.get(f"http://{ESP32_IP_ADDRESS}/LED/on")
        if response.status_code == 200:
            self.label_status.config(text="LED Status: ON")
        else:
            self.label_status.config(text="Error turning LED on")

    def turn_led_off(self):
        response = requests.get(f"http://{ESP32_IP_ADDRESS}/LED/off")
        if response.status_code == 200:
            self.label_status.config(text="LED Status: OFF")
        else:
            self.label_status.config(text="Error turning LED off")

if __name__ == "__main__":
    root = tk.Tk()
    app = LedControlApp(root)
    root.mainloop()
