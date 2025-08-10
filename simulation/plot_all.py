import sys

import matplotlib.dates as mdates
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.widgets import CheckButtons

if len(sys.argv) < 2:
    print("Usage: python plot_all.py data.csv")
    sys.exit(1)

filename = sys.argv[1]

# Optimized CSV reading
df = pd.read_csv(filename, parse_dates=['Time'], date_format='%H:%M:%S')

# Multiply relay columns by 50 (vectorized operation)
relay_cols = [col for col in df.columns if col.startswith("R")]
if relay_cols:
    df[relay_cols] *= 50

# Convert time to numeric for faster plotting
time_numeric = mdates.date2num(df["Time"])

# Prepare colors & styles
color_cycle = plt.cm.tab20.colors
line_styles = ['-', '--', '-.', ':']

# Pre-configure matplotlib for better performance
plt.rcParams['path.simplify'] = True
plt.rcParams['path.simplify_threshold'] = 0.1
plt.rcParams['agg.path.chunksize'] = 10000

fig, ax = plt.subplots(figsize=(14, 7))
plt.subplots_adjust(left=0.2)  # Make space for checkboxes

# Pre-allocate arrays for better performance
lines = []
labels = []
visibility = []
data_columns = [col for col in df.columns if col != "Time"]

# Batch plotting for better performance
for i, col in enumerate(data_columns):
    color = color_cycle[i % len(color_cycle)]
    
    # Use plot with markers and lines (no interpolation, just connects existing points)
    line, = ax.plot(time_numeric, df[col], label=col, 
                   color=color, linestyle='-', marker='o', markersize=4, 
                   linewidth=1, alpha=0.8)
    
    lines.append(line)
    labels.append(col)
    visibility.append(True)

# Optimize axis formatting
ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M:%S'))

# Reduce number of x-ticks for better performance
max_ticks = min(20, len(df))  # Limit to 20 ticks maximum
tick_indices = np.linspace(0, len(df)-1, max_ticks, dtype=int)
ax.set_xticks(time_numeric[tick_indices])

# Set limits and labels
ax.set_ylim(0, 99)
ax.set_xlabel("Time (HH:MM:SS)")
ax.set_ylabel("Value")
ax.set_title("All Sensor Readings (No Interpolation)")
ax.grid(True, alpha=0.3)  # Lighter grid for better performance

# Optimized checkbox handling with Show/Hide All
rax = plt.axes([0.01, 0.1, 0.15, 0.8])

# Add "Show/Hide All" as first option
all_labels = ["Show/Hide All"] + labels
all_visibility = [True] + visibility

check = CheckButtons(rax, all_labels, all_visibility)

# More efficient visibility toggle
def func(label):
    if label == "Show/Hide All":
        # Toggle all plots
        current_state = lines[0].get_visible() if lines else True
        new_state = not current_state
        
        for line in lines:
            line.set_visible(new_state)
        
        # Update all checkboxes except the first one
        for i in range(1, len(all_visibility)):
            check.set_active(i, new_state)
            
    else:
        # Toggle individual plot
        index = labels.index(label)
        lines[index].set_visible(not lines[index].get_visible())
    
    fig.canvas.draw_idle()  # More efficient than plt.draw()

check.on_clicked(func)

# Enable blitting for faster redraws
fig.canvas.draw()
plt.show()