@echo off
if exist "node_modules" (
 start npm start
) else (
 start npm run build-start
)
start http://localhost:3000