# ece362-proj
### {Typing Test}
1. Reset the controller
2. Press space to start
3. Wait for high score:)
   
### Source Files 
- `sdcard.c`: provides functions to read and parse files from an sd card (in this case, we have stored high score and typing prompts)
- `gamelogic.c`: provides logic for non-typing game logic
- `ps2.c`: configures TIM1 as a slave in order to respond to a key press
- `timer.c`, `interrupt.c`: provides 15s countdown timer, and necessary game logic to execute at specific times
- `dac.c`: responsible for outputting a wrong sound when a wrong key is pressed.
