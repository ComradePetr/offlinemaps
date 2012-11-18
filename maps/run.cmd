@echo off

setlocal enableDelayedExpansion

for %%i in (%1) do (
	echo -------------------------
	echo START %%i in !TIME!
	Project.exe %%i
	type output.txt
	echo END %%i in !TIME!
	echo -------------------------
)