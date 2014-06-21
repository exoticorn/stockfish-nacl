/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2013 Marco Costalba, Joona Kiiski, Tord Romstad

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <sstream>
#include <string>

#include "bitboard.h"
#include "evaluate.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "tt.h"
#include "ucioption.h"

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

void initialize() {
	std::cout << engine_info() << std::endl;

	UCI::init(Options);
	Bitboards::init();
	Position::init();
	Bitbases::init_kpk();
	Search::init();
	Pawns::init();
	Eval::init();
	Threads.init();
	TT.resize(Options["Hash"]);
	UCI::initialize();
}

class CaptureBuf : public std::streambuf {
public:
	CaptureBuf(pp::Instance* inst) {
		instance = inst;
		pos = 0;
	}
protected:
	pp::Instance* instance;
	char buffer[8192];
	int pos;
	virtual int overflow(int c) {
		if(c == '\n') {
			buffer[pos] = 0;
			pos = 0;
			instance->PostMessage(pp::Var(buffer));
		} else if(pos < sizeof(buffer) - 2) {
			buffer[pos++] = c;
		}
		return 0;
	}
};

class StockfishInstance : public pp::Instance {
public:
	explicit StockfishInstance(PP_Instance instance) : pp::Instance(instance), captureBuf(this) {
		origBuf = std::cout.rdbuf(&captureBuf);
		origBufErr = std::cerr.rdbuf(&captureBuf);
		initialize();
	}
	virtual ~StockfishInstance() {
		std::cout.rdbuf(origBuf);
		std::cerr.rdbuf(origBufErr);
	}

	virtual void HandleMessage(const pp::Var& var_message) {
		if(!var_message.is_string())
			return;
		std::string message = var_message.AsString();
		UCI::command(message);
	}
private:
	CaptureBuf captureBuf;
	std::streambuf* origBuf;
	std::streambuf* origBufErr;
};

class StockfishModule : public pp::Module {
public:
	StockfishModule() : pp::Module() {}
	virtual ~StockfishModule() {}

	virtual pp::Instance* CreateInstance(PP_Instance instance) {
		return new StockfishInstance(instance);
	}
};

namespace pp {
	Module* CreateModule() {
		return new StockfishModule();
}
}
