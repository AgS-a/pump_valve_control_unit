import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

file_name = 'calibration.ods'

df = pd.read_excel(file_name, engine='odf')

x = df['Pressure(mubar)']
y = df['Voltage(V)']

log_x = np.log10(x)

m, c = np.polyfit(log_x, y, 1)

print(m, c)
x_fit = np.geomspace(x.min(), x.max(), 100)
y_fit = m * np.log10(x_fit) + c

plt.figure(figsize=(8, 6))
plt.plot(x, y, '.')
plt.plot(x_fit, y_fit)

plt.xscale('log')
plt.title('Voltage vs. Pressure')
plt.xlabel('Pressure (mubar)')
plt.ylabel('Voltage (V)')
plt.grid(True, linestyle='--', alpha=0.7)

plt.show()
