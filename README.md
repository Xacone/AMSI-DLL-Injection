# A crafted/AMSI-Bypassing injectable DLL for establishing an undetected Reverse Shell using Microsoft Detours.

<b>This tool allows to bypass Microsoft's AMSI interface (Windows Defender base) by going through <a href="https://www.microsoft.com/en-us/research/project/detours/">Microsoft Detours</a> in order to intercept and hijack the function calls that emanate from it, so as to launch an undetectable reverse shell and take control of the victim's machine.
This is obviously only a POC and this tool/method can be extended to an infinite number of possibilities.</b>

<h3> Injection on notepad.exe & Reverse shell : </h3>
<img src="rs.gif">

<h3> Normal Scenario : </h3>
<img width="60%" src="https://i.ibb.co/wczdjVH/image.png">

<h3> DLL injection through & AMSI bypassing through Detours : </h3>
<img width="90%" src="https://i.ibb.co/S7ckJB6/amsi-1.png">

Crafted module loaded among the others : <br>
<img width="70%" src="https://i.ibb.co/z6hyWff/image.png">

<b><i>Sources : </i><br>
<a href="https://x64sec.sh/understanding-and-bypassing-amsi/">Understanding and bypassing AMSI</a><br>
<a href="https://github.com/microsoft/Detours/wiki/OverviewInterception">OverviewInterception</a><br>
<a href="https://github.com/microsoft/Detours">Microsoft Detours Package</a><br>
<a href="https://github.com/Xacone/NTDLLUnhookedReverseShell">NTDLL Unhooked Reverse Shell</a>
</b>
