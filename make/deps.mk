
$(BUILD_DIR)/buildit.dep: $(BUILDIT_DEPS) 
	$(MAKE) -C $(BUILDIT_DIR) DEBUG=$(DEBUG)
	touch $(BUILD_DIR)/buildit.dep
