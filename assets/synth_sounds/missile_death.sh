attack_slope = 10000     # 10 slow, 1000 fast
decay_slope = 10000      # 10 slow, 1000 fast
sustain_vol = 100     # 0 silent, 100 full
sustain_time = 0     # in samples
release_slope = 100    # 10 slow, 1000 fast

# Main signal generator unit
osc1_type = triangle       # triangle | saw | square | noise
osc1_freq = 80        # hz
osc1_freq_vel = -12    # hz / 200

# Secondary signal generator unit (modulates the signal from the first)
# osc2_type = saw        # triangle | saw | square | noise
# osc2_freq = 50        # hz
# osc2_freq_vel = -5        # hz

#modulation = add # multiply | add | fm

#lowpass_freq = 1000     # 0 to 20000 hz
# lowpass_freq_vel = -80
#lowpass_res = 90         # 100 high resonance, 0 low resonance
