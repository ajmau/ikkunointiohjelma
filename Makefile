test:
	Xephyr -ac -screen 1280x1024 -br -reset -terminate 2> /dev/null :1 &
	sleep 1
	env DISPLAY=:1 ./main
