.PHONY: clean All

All:
	@echo "----------Building project:[ server - Release ]----------"
	@cd "server" && "$(MAKE)" -f  "server.mk" && "$(MAKE)" -f  "server.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ server - Release ]----------"
	@cd "server" && "$(MAKE)" -f  "server.mk" clean
