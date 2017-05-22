#pragma once


#define VISITABLE public: virtual void accept (numbat::visitor::BaseVisitor & vis) {acceptVisitor (*this, vis);}
#define CONST_VISITABLE public: virtual void accept (numbat::visitor::BaseVisitor & vis) const {acceptVisitor (*this, vis);}

namespace numbat {
namespace visitor {


class BaseVisitor {
	public:
		virtual ~BaseVisitor () {}
};

template <typename T>
class ConstVisitor : public virtual BaseVisitor {
	public:
		virtual void visit (const T & target)=0;
		virtual ~ConstVisitor () {}
};

template <typename T>
class Visitor : public virtual BaseVisitor {
	public:
		virtual void visit (T & target)=0;
		virtual ~Visitor () {}
};

class BaseConstVisitable {
	public:
		virtual void accept (BaseVisitor & vis) const=0;
		virtual ~BaseConstVisitable () {}
	protected:
		template <typename T>
		static void acceptVisitor (const T & target, BaseVisitor & vis) {
			dynamic_cast <ConstVisitor <T> &> (vis).visit (target);
		}
};

class BaseVisitable {
	public:
		virtual void accept (BaseVisitor & vis) {};
		virtual ~BaseVisitable () {}
	protected:
		template <typename T>
		static void acceptVisitor (T & target, BaseVisitor & vis) {
			dynamic_cast <Visitor <T> &> (vis).visit (target);
		}
};


}
}
