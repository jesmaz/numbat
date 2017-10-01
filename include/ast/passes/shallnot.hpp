#pragma once


#include <ast/passes/abstract.hpp>
#include <ast/type.hpp>


namespace AST {


class ShallNotPass : public AbstractPass {
	
	public:
		
		virtual NodePtr visit (const NodePtr & node)=0;
		
		virtual void visit (const And &) {abort ();}
		virtual void visit (const Array &) {abort ();}
		virtual void visit (const ArrayInit &) {abort ();}
		virtual void visit (const ArrayVal &) {abort ();}
		virtual void visit (const Basic_Operation &) {abort ();}
		virtual void visit (const Call_0 &) {abort ();}
		virtual void visit (const Call_1 &)  {abort ();}
		virtual void visit (const Call_2 &)  {abort ();}
		virtual void visit (const Call_n &)  {abort ();}
		virtual void visit (const CastToArbPrec &) {abort ();}
		virtual void visit (const CastToInt &) {abort ();}
		virtual void visit (const CastToDecimal &) {abort ();}
		virtual void visit (const CastToFloat &) {abort ();}
		virtual void visit (const CastToUint &) {abort ();}
		virtual void visit (const Const &)  {abort ();}
		virtual void visit (const Function_Ptr &)  {abort ();}
		virtual void visit (const Function_Set &)  {abort ();}
		virtual void visit (const IfElse &)  {abort ();}
		virtual void visit (const Inferred &)  {abort ();}
		virtual void visit (const Interface &)  {abort ();}
		virtual void visit (const Number &)  {abort ();}
		virtual void visit (const Numeric &)  {abort ();}
		virtual void visit (const Or &) {abort ();}
		virtual void visit (const Ref &)  {abort ();}
		virtual void visit (const Reflect &) {abort ();}
		virtual void visit (const ReflectType &) {abort ();}
		virtual void visit (const Sequence &)  {abort ();}
		virtual void visit (const Struct &)  {abort ();}
		virtual void visit (const Unresolved_Call &)  {abort ();}
		virtual void visit (const Unresolved_Constructor &)  {abort ();}
		virtual void visit (const Unresolved_IfElse &)  {abort ();}
		virtual void visit (const Unresolved_Operation &)  {abort ();}
		virtual void visit (const Variable &)  {abort ();}
		
	protected:
	private:
		
		
		
};


}

