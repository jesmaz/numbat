#pragma once


#include <codegen/target.hpp>
#include <nir/inst/add.hpp>
#include <nir/inst/alloc.hpp>
#include <nir/inst/bitwise.hpp>
#include <nir/inst/call.hpp>
#include <nir/inst/composite.hpp>
#include <nir/inst/constant.hpp>
#include <nir/inst/div.hpp>
#include <nir/inst/equal.hpp>
#include <nir/inst/functionPointer.hpp>
#include <nir/inst/get.hpp>
#include <nir/inst/jump.hpp>
#include <nir/inst/less.hpp>
#include <nir/inst/lessequal.hpp>
#include <nir/inst/mul.hpp>
#include <nir/inst/nequal.hpp>
#include <nir/inst/neg.hpp>
#include <nir/inst/ptradd.hpp>
#include <nir/inst/put.hpp>
#include <nir/inst/ret.hpp>
#include <nir/inst/reinterpret.hpp>
#include <nir/inst/sub.hpp>
#include <nir/parameter.hpp>
#include <nir/type/number.hpp>
#include <nir/type/struct.hpp>
#include <visitor.hpp>


namespace codegen {

using numbat::visitor::ConstVisitor;

class TargetVisitor :
	public Target,
	public ConstVisitor <Add>,
	public ConstVisitor <Alloc>,
	public ConstVisitor <BitAnd>,
	public ConstVisitor <BitNot>,
	public ConstVisitor <BitOr>,
	public ConstVisitor <BitXor>,
	public ConstVisitor <Composite>,
	public ConstVisitor <Constant>,
	public ConstVisitor <Div>,
	public ConstVisitor <DirectCall>,
	public ConstVisitor <Equal>,
	public ConstVisitor <FunctionPointer>,
	public ConstVisitor <Get>,
	public ConstVisitor <Jump>,
	public ConstVisitor <Less>,
	public ConstVisitor <LessEqual>,
	public ConstVisitor <Mul>,
	public ConstVisitor <Neg>,
	public ConstVisitor <NEqual>,
	public ConstVisitor <Number>,
	public ConstVisitor <Parameter>,
	public ConstVisitor <PtrAdd>,
	public ConstVisitor <Put>,
	public ConstVisitor <Reinterpret>,
	public ConstVisitor <Ret>,
	public ConstVisitor <Struct>,
	public ConstVisitor <Sub> {};

}
