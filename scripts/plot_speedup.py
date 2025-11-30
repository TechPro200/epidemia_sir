# plot_speedup.py
import pandas as pd
import matplotlib.pyplot as plt
df = pd.read_csv('results/speedup.csv')
plt.figure()
plt.plot(df['threads'], df['speedup'], marker='o')
plt.xlabel('Threads')
plt.ylabel('Speed-up')
plt.grid(True)
plt.savefig('results/speedup.png', dpi=150)
print('Saved results/speedup.png')