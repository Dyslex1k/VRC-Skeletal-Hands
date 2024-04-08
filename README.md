# VRC Skeletal Hands
This is an OSC based Solution to Skeletal Hand tracking in VRChat developed by Dyslex1k and with a lot of help form hyblocker. This was a small app put together in C++ that sends Curl and Splay parameters through OSC to allow Skeletal Hand tracking before VRChats Implemetaion goes live. This will work with any device that supports skeletal input though the quality may vary from controller to controller!

### Demo with Valve Knuckles
https://github.com/Dyslex1k/VRC-Skeletal-Hands/assets/102997160/c82dad26-7023-4138-a80e-bdbb67e895fd


# How to set up
1) Add the unity package to your project.
2) Add the VRCFury/ Modular Avatar Prefab to your model in the Highrachy.
3) When in vr Launch the app.
4) Enable the Skeletal input in the quick menu.

## Syncing In VRChat
Currently syncing the parameters requires 144bits of data (This is too much) I figured out a potential way of halving it though I may not develop due to information about better input for VRChat coming soon. Though if you want
you want to develop a solution please do and create a Pull Request.
### So what was the plan?
I spoke with a friend called nesterboy and came up with this solution:
- Each parameter will have 4 bools each (This would be like Binary)
- These parameters will use a blend tree in the FX layer to do the maths to convert them to float values
- These float values will then be used on a remote parameter like: SH/Remote/Left/Curl/index
- Osc smooth is then applied to hide the lowered acuracy

You can see an example of the Blend tree for 1 parameter here:

![image](https://github.com/Dyslex1k/VRC-Skeletal-Hands/assets/102997160/cf542f93-444c-40df-a216-1ed86185071f)
 
This is the current Idea on how this would work though due to my personal life and VRChats solution being "Very Close" I will not spend to making this. Though if VRChat hasn't implemeted this in a month I will work on making this solution a reality!
## Known Issues
- When SteamVr Overlay is up all hand tracking stops working.
- Touch Controllers have no gesture system.
- Does not support upper and lower finger curl. (DiverX Gloves)
- Currently uses a large about of bit space to sync remote users.
