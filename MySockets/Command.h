#pragma once

struct command {
	int count = 0;
	virtual void execute(Node* node, std::vector<std::string> params) const = 0;
};