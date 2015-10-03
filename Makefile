.PHONY: clean All

All:
	@echo "----------Building project:[ server - Debug ]----------"
	@cd "server" && "$(MAKE)" -f  "server.mk" && "$(MAKE)" -f  "server.mk" PostBuild
clean:
	@echo "----------Cleaning project:[ server - Debug ]----------"
	@cd "server" && "$(MAKE)" -f  "server.mk" clean
