@echo off
git add .
git commit -m "auto update"
git remote set-url origin https://ghproxy.com/https://github.com/Yuuuuu1725/breakout.git
git push origin main
pause
