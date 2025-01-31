# -*- coding: utf-8 -*-
"""
Created on Mon Sep 16 15:56:10 2024

@author: Braedon Dority
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

df_amps = pd.read_csv('AMP.CSV', skiprows=1, nrows=12990)
df_amps.index = df_amps.DateTime

mAmp_sec = np.trapz(df_amps['Amps (mA)'], df_amps['Time (sec)'])
sec_measured = 12997
ave_mAmp = mAmp_sec / sec_measured
days_needed = 7
hours_needed = days_needed * 24
battery_req = ave_mAmp * hours_needed / 1000
