# raditecSBG
Radikal Technologies (raditec) Spectralis editor. View and edit (groove and line sequence) data stored in `.SPT` files. Depends on my [raditecSPT](/zenpho/raditecSPT) library.

## What is this? Why is it useful?

I made this because I find the step-sequencers on the Spectralis very easy to use, but cumbersome to copy sequence data around between GROOVE and LINE sequence engines, and it is not simple to route sequences to external MIDI hardware.  

raditecSBG makes all of these tasks quick and simple, it is a joyful experience to try out musical ideas, move them between the sequencer engines on the Spectralis hardware and easily route them to external MIDI gear in seconds.

Here's a screenshot of the software running. It runs in a terminal window with `ncurses`.

```
----------- v0.0 - PATT12 - mode:transfer
┌Commands─┐┌Scrollable─────────────────────────────────────────────────────────┐
│p setpath││CWD:/Volumes/SPC_FLASH/SONG01                                      │
│,.  SPT+-││NAME:<PattXX>                                                      │
│         ││kick:[==.=....====....] L01:[....xxxx........] (prc3)              │
│r rhythms││snar:[................] L02:[....xxxx........] (fb,89 0a,00)       │
│as  grv+-││clhh:[===.=.=.=.=.====] L03:[................] (ee,0c 00,00)       │
│AS  lin+-││ophh:[................] L04:[................]                     │
│         ││clap:[....=.......=...] L05:[................]                     │
│xX  clear││htom:[=......=..=.....] L06:[................]                     │
│cC   copy││mtom:[................] L07:[................]                     │
│         ││ltom:[=..=..=.=...=..=] L08:[................]                     │
│Ee extint││prc1:[................] L09:[................]                     │
│         ││prc2:[................] L10:[................]                     │
│Iui   Ils││prc3:[=...............] L11:[................]                     │
│kl   Il+-││                                                                   │
│         ││SRC00 (press a/s) ----- TGT00 (press A/S)                          │
│q exit   ││kick:[==.=....====....] L01:[....xxxx........] (prc3)              │
│         ││                                                                   │
│         ││                                                                   │
│         ││                                                                   │
│         ││                                                                   │
└─────────┘└───────────────────────────────────────────────────────────────────┘
```

## How do I use it?

Set your Spectralis to `USB MODE` and raditecSBG will automatically search for `/Volumes/SPC_FLASH`. You can push any key to retry if you need to plug in the USB cable, or push 'q' to abort.

Use the 'p' key to select the path (`SONG01` or `SONG02` etc) and '.' and ',' keys to count up and down to focus on `PAT01` `PAT02` etc within a song. Capital and lowercase letters like 'a' and 'A', 's' and 'S' count up and down to focus on groove and line sequence parts. Use 'E' and 'e' keys to (de)activate external MIDI note triggering for parts.

Raw data is viewable in hexadecimal or decimal format by using the 'i' 'I' 'u' 'k' and 'l' keys.

```
----------- v0.0 - PATT01 - mode:help info
┌Commands─┐┌Scrollable─────────────────────────────────────────────────────────┐
│p setpath││Radikal Technologies Spectralis SPT editor (zenpho.co.uk)     │
│,.  SPT+-││==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====   │
│         ││This thing kludge-tastically modifies Groove and Line Sequences    │
│r rhythms││                                                                   │
│as  grv+-││Use arrow up/down and page up/down keys to scroll                  │
│AS  lin+-││Use command keys listed in the left panel                          │
│         ││                                                                   │
│xX  clear││Typical workflow:                                                  │
│cC   copy││  - 'p' setpath /Volumes/SPC_FLASH/SONG01/                         │
│         ││  - ',' and '.' keys to select PATT##                              │
│Ee extint││  - 'r' key views drum groove + lineseq triggers                   │
│         ││  - 'a' and 's' keys to select +- groove                           │
│Iui   Ils││  - 'A' and 'S' keys to select +- lineseq                          │
│kl   Il+-││  - 'x' and 'X' key clears selected                                │
│         ││  - 'c' key copies groove to line, overwriting PATT##.SPT          │
│q exit   ││  - 'C' key copies line to groove, overwriting PATT##.SPT          │
│         ││                                                                   │
│         ││                                                                   │
│         ││                                                                   │
│         ││                                                                   │
└─────────┘└───────────────────────────────────────────────────────────────────┘
```

## Important

This is an in-place editor, no backups performed, data may be destroyed by accident.

## Acknowledgements

raditecSBG uses the `ncurses` library and [raditecSPT](/zenpho/raditecSPT) libraries to display, read, and write Radikal Technologies Spectralis SPT files.
