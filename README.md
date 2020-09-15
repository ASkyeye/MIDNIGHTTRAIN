# MIDNIGHTTRAIN
[![](https://img.shields.io/badge/Category-Persistence-E5A505?style=flat-square)]() [![](https://img.shields.io/badge/Language-C%20%2f%20C++%20%2f%20Python3-E5A505?style=flat-square)]()

Covert Stage-3 Persistence Framework utilizing NVRAM variables

## Read About It Here
[![](https://img.shields.io/badge/MIDNIGHTTRAIN-E5A505?style=flat-square)](https://slaeryan.github.io/posts/midnighttrain.html)

## Warning
This is a PoC framework and as such will not be maintained. Sorry!

<p align="center">
  <img src="itjustworks.jpg">
  <figcaption>lol</figcaption>
</p>

It has been made as a small weekend project and has received limited testing ergo, bugs/undefined behaviour is to be expected. However, I am willing to fix critical bugs in my spare time should you encounter them so feel free to open an issue.

It supports x64 implants only and all testing was done on:
- Windows 10 x64 version 1903
- Windows 10 x64 version 2004

## Usage
Here's a guide to testing the framework in 10 easy steps:

```
1. Make sure you have a working VC++ 2019 dev environment set up beforehand.
2. Place your 64-bit Staged Meterpreter/Beacon shellcode payload in `Bin` as `payload_x64.bin`. You might need to use sRDI to convert DLLs to PIC blob if your framework doesn't support the generation of Staged payloads as shellcode.
3. Open an x64 Developer Command Prompt.
4. git clone https://github.com/slaeryan/MIDNIGHTTRAIN.git - To clone the repository.
5. cd MIDNIGHTTRAIN & cd Gremlin & compile64.bat - To build the Gremlin implant.
6. cd .. & cd Gargoyle & compile64.bat - To build the Gargoyle implant.
7. You'll find two compiled implant DLLs in the `Bin` folder named `gremlin_x64.dll` and `gargoyle_x64.dll`.
8. cd Python & python ConvertToShellcode.py ../Bin/gargoyle_x64.dll - To convert Gargoyle DLL to PIC blob.
9. cd .. & cd Scratchpad & compile64.bat - To build the loader for inline execution of shellcode blob.
10. Finally to test, loader <path-to-gargoyle_x64.bin>
```

## Author
Upayan ([@slaeryan](https://twitter.com/slaeryan)) [[slaeryan.github.io](https://slaeryan.github.io)]

## Caveats
- Need an elevated context to install persistence.
- Maximum permissible size of payload usable with this framework is ~`36 kB`. Need to craft a custom stager within the size limit to use Stageless payloads.

## Credits
1. [https://github.com/perturbed-platypus](https://github.com/perturbed-platypus) - Big thanks to [@TTimzen](https://twitter.com/ttimzen?lang=en) & [@r00tkillah](https://twitter.com/r00tkillah?lang=en) for their wonderful research.
2. [https://gist.github.com/jthuraisamy/e602d5d870230df3ce00178001f9ac16](https://gist.github.com/jthuraisamy/e602d5d870230df3ce00178001f9ac16) - Another PoC thanks to [@Jackson_T](https://twitter.com/jackson_t?lang=en)
3. [Sektor7 RTO: Windows Persistence Course](https://institute.sektor7.net/rto-windows-persistence) - For the Port Monitor DLL snippet.
4. [Sektor7 RTO: Malware Development Essentials](https://institute.sektor7.net/red-team-operator-malware-development-essentials) & [reenz0h](https://twitter.com/Sektor7Net) - For the wonderful initiation into the world of implant development and the templates that I still use to this date.
5. [@am0nsec](https://twitter.com/am0nsec?lang=en) for dropping dem hints regarding the token impersonation.
6. [CIA Vault7 leaks](https://wikileaks.org/ciav7p1/cms/page_31227915.html) - I have a joke but it is [REDACTED]().
7. [@monoxgas](https://twitter.com/monoxgas?lang=en) for sRDI and being an awesome researcher in general!
8. [Mr. Base64]() - for the review and code improvements. +1 for being a top-level guy! You can find him hanging out here [0x00sec Discord](https://discord.com/invite/c6BHVfn) with a bunch of other really cool peeps.

## License
All the code included in this project is licensed under the terms of the GNU GPLv2 license.

#

[![](https://img.shields.io/badge/slaeryan.github.io-E5A505?style=flat-square)](https://slaeryan.github.io) [![](https://img.shields.io/badge/twitter-@slaeryan-00aced?style=flat-square&logo=twitter&logoColor=white)](https://twitter.com/slaeryan) [![](https://img.shields.io/badge/linkedin-@UpayanSaha-0084b4?style=flat-square&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/upayan-saha-404881192/)
