import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

#file_name = 'calibration_1-to-500_microbar.ods'
file_name = 'pirani_pressure_vs_voltage.ods'

df = pd.read_excel(file_name, engine='odf')

x = df['Pressure(mubar)']
y = df['Voltage(V)']

log_x = np.log10(x)

#m, c = np.polyfit(log_x, y, 1)
a1, a2, a3, a4, a5, a6, a7 = np.polyfit(log_x, y, 6)
#print(a1, a2, a3, a4, a5, a6, a7)

#print(m, c)
x_fit = np.geomspace(10, 1100000, 200)
#y_fit = m * np.log10(x_fit) + c
y_fit = (a1 * (np.log10(x_fit))**6) + (a2 * (np.log10(x_fit))**5) +(a3 * (np.log10(x_fit))**4) +(a4 * (np.log10(x_fit))**3) + (a5 * (np.log10(x_fit))**2) + (a6 * np.log10(x_fit)) + a7 

plt.figure(figsize=(8, 6))
plt.plot(x, y, '.')
plt.plot(x_fit, y_fit,'.')

result = ", ".join(f"{num:.4f}" for num in y_fit)

print(result)

plt.xscale('log')
plt.title('Voltage vs. Pressure')
plt.xlabel('Pressure (mubar)')
plt.ylabel('Voltage (V)')
plt.grid(True, linestyle='--', alpha=0.7)

plt.show()
