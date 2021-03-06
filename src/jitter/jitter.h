/* Copyright (c) 2018-2019 Hans-Kristian Arntzen
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include <memory>
#include <unordered_map>

namespace JITTIR
{
class Jitter
{
public:
	Jitter();

	static void init_global();

	using ModuleHandle = llvm::orc::VModuleKey;

	ModuleHandle add_module(std::unique_ptr<llvm::Module> module);

	void remove_module(ModuleHandle handle);

	llvm::JITSymbol find_symbol(const std::string &name);

	llvm::JITTargetAddress get_symbol_address(const std::string &name);

	std::unique_ptr<llvm::Module> create_module(const std::string &name);

	template <typename T>
	void add_external_symbol(const std::string &name, T sym)
	{
		add_external_symbol_generic(name, (uint64_t) sym);
	}

	void add_external_symbol_generic(const std::string &name, uint64_t symbol);

	void set_external_ir_dump_directory(const std::string &dir);

	void enable_log_module(bool enable)
	{
		log_module = enable;
	}

	void enable_optimize_module(bool enable)
	{
		optimize_module = enable;
	}

	void enable_validate_module(bool enable)
	{
		validate_module = enable;
	}

private:
#ifdef JITTER_LLVM_VERSION_LEGACY
	llvm::LLVMContext context;
#else
	llvm::orc::ThreadSafeContext context;
#endif
	std::unique_ptr<llvm::orc::ExecutionSession> execution_session;

#ifdef JITTER_LLVM_VERSION_LEGACY
	std::unique_ptr<llvm::orc::RTDyldObjectLinkingLayer> object_layer;
	std::unique_ptr<llvm::orc::IRCompileLayer<
		llvm::orc::RTDyldObjectLinkingLayer,
		llvm::orc::SimpleCompiler>> compile_layer;
#else
	std::unique_ptr<llvm::orc::LegacyRTDyldObjectLinkingLayer> object_layer;
	std::unique_ptr<llvm::orc::LegacyIRCompileLayer<
		llvm::orc::LegacyRTDyldObjectLinkingLayer,
		llvm::orc::SimpleCompiler>> compile_layer;
#endif

	std::unique_ptr<llvm::TargetMachine> target_machine;
	std::unique_ptr<llvm::orc::MangleAndInterner> mangler;
	std::unique_ptr<llvm::DataLayout> data_layout;
	std::unordered_map<std::string, uint64_t> externals;
	std::string ir_dump_dir;
	bool log_module = false;
	bool optimize_module = false;
	bool validate_module = false;
};
}