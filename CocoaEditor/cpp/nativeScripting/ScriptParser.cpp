#include "externalLibs.h"
#include "nativeScripting/ScriptParser.h"

#include "cocoa/util/Log.h"
#include "cocoa/core/Memory.h"

namespace Cocoa
{
	// Internal variables
	static std::vector<Token> Tokens;
	static int CurrentToken = 0;

	struct AstNode;
	enum class AstNodeType
	{
		None,
		BracedInitList,
		InitList,
		InitializerClause,
		InitializerList,
		OperatorFunctionId,
		TypeParameter,
		BinaryExpression,
		TernaryExpression,
		AssignmentExpression,
		Expression,
		PointerToMember,
		CastExpression,
		UnaryExpression,
		SizeofExpression,
		SizeofIdentifierExpression,
		AlignofExpression,
		Delete,
		Literal,
		This,
		Grouping,
		LambdaExpression,
		LambdaIntroducer,
		LambdaCapture,
		Capture,
		LambdaCaptureList,
		LambdaDeclarator,
		TemplateArgumentList,
		TemplateQualifiedId,
		TypeId,
		EnumName,
		EnumSpecifier,
		EnumKey,
		EnumHead,
		OpaqueEnumDecl,
		EnumBase,
		EnumeratorList,
		EnumeratorDefinition,
		ConstantExpression,
		IfElse,
		Switch,
		InitializerCondition,
		BracedInitCondition,
		WhileLoop,
		DoWhileLoop,
		ForLoop,
		ForEachLoop,
		ForRangeDeclaration,
		ForRangeInitializer,
		Statement,
		QualifiedId,
		LabeledIdentifier,
		CaseLabel,
		DefaultLabel,
		CompoundStatement,
		StatementSequence,
		Break,
		Continue,
		Return,
		Goto,
		NamespaceName,
		NamedNamespaceDefinition,
		UnnamedNamespaceDefinition,
		NamespaceAliasDefinition,
		QualifiedNamespaceSpecifier,
		UsingDeclaration,
		UsingTypenameDeclaration,
		UsingDirective,
		Asm,
		LinkageSpecificationBlock,
		LinkageSpecification,
		AttributeSpecifierSequence,
		AlignAsTypeId,
		AlignAsExpression,
		AttributeList,
		EmptyAttributeList,
		Attribute,
		AttributeToken,
		AttributeArgumentClause,
		BalancedTokenSeq,
		BalancedToken,
		InitDeclaratorList,
		InitDeclarator,
		Declarator,
		PtrDeclarator,
		ParametersAndQualifiers,
		TrailingReturnType,
		PtrStar,
		Ref,
		RefRef,
		PtrNamespaceStar,
		CvQualifierSeq,
		CvQualifier,
		RefQualifier,
		DeclaratorId,
		ClassName,
		ClassSpecifier,
		ClassHead,
		ClassVirtualHead,
		ClassHeadName,
		ClassVirtSpecifierSeq,
		ClassVirtSpecifier,
		ClassKey,
		MemberAndAccessSpecifier,
		MemberSpecifier,
		MemberFunctionDeclaration,
		MemberDeclaration,
		MemberDeclaratorList,
		MemberDeclaratorPure,
		MemberDeclaratorBrace,
		MemberDeclarator,
		VirtSpecifierSeq,
		VirtSpecifier,
		PureSpecifier,
		BaseSpecifierList,
		BaseSpecifier,
		ClassOrDecltype,
		AccessSpecifier,
		ConversionFunctionId,
		ConversionTypeId,
		ConversionDeclarator,
		MemInitializerList,
		MemExpressionInitializer,
		MemBracedInitializer,
		MemInitializerId,
		NestedNamespaceSpecifierId,
		NestedNamespaceSpecifierTemplate,
		PostfixSimpleTypeExpressionList,
		PostfixSimpleTypeBraceList,
		PostfixTypenameSpecExpressionList,
		PostfixTypenameSpecBraceList,
		PostfixCast,
		PostfixTypeIdExpression,
		PostfixTypeId,
		PostfixBracketExpression,
		PostfixBracketBraceList,
		PostfixParenExpressionList,
		PostfixMemberIdExpression,
		PostfixPseudoDestructor,
		PostfixPlusPlus,
		PostfixMinusMinus,
		PseudoDestructorDecltype,
		PseudoDestructorTemplate,
		PseudoDestructor,
		PseudoNestedDestructor,
		NewTypeIdExpression,
		NewExpression,
		NewPlacement,
		NewTypeId,
		NewDeclarator,
		NoptrNewTailDeclarator,
		NoptrNewDeclarator,
		NewInitializer,
		DeclarationSeq,
		AliasDeclaration,
		SimpleDeclaration,
		StaticAssertDeclaration,
		SimpleDeclSpecifier,
		DeclSpecifier,
		DeclSpecSeq,
		StorageClassSpec,
		FunctionSpec,
		TypedefName,
		TypeSpecSeq,
		TrailingTypeSpecSeq,
		SimpleTypeTokenSpec,
		SimpleTypeTemplateSpec,
		SimpleTypeSpec,
		DecltypeSpec,
		AbstractDeclarator,
		AbstractElipsisDeclarator,
		PtrAbstractDeclarator,
		ParameterDeclarationList,
		ParameterDefaultDeclaration,
		ParameterDeclaration,
		ParameterAbstractDefaultDeclaration,
		ParameterAbstractDeclaration,
		FunctionDefaultDefinition,
		FunctionDefinition,
		FunctionBody,
		LiteralOperatorId,
		TemplateDeclaration,
		TemplateParameterList,
		SimpleTemplateId,
		LiteralOperatorTemplateId,
		FunctionOperatorTemplateId,
		TemplateName,
		TypenameSpecifier,
		TypenameTemplateSpecifier,
		ExplicitInstantiation,
		TryBlock,
		FunctionTryBlock,
		HandlerSeq,
		Handler,
		ExceptionDeclaration,
		ExceptionAbstractDeclaration,
		ThrowExpression,
		DynamicExceptionSpec,
		TypeIdList,
		NoexceptSpec,
		NoexceptExpressionSpec,
		PreprocessingFile,
		NoptrAbstractDeclarator,
		NoptrAbstractExpressionDeclarator,
		NoptrDeclarator,
		NoptrConstantDeclarator,
		UnqualifiedId,
		UnqualifiedIdDtorClass,
		UnqualifiedIdDtorDecltype,
		ElaboratedSpecifierEnum,
		ElaboratedSpecifierClass,
		ElaboratedSpecifierTemplate,
		AlignmentExpression,

		// Preprocessing Stuff
		Group,
		IfSection,
		IfGroup,
		IfDefGroup,
		IfNDefGroup,
		ElifGroups,
		ElifGroup,
		ElseGroup,
		MacroInclude,
		MacroDefine,
		MacroDefineFunction,
		MacroUndef,
		MacroLine,
		MacroError,
		MacroPragma,
		TextLine,
		NonDirective,
		Identifier,
		IdentifierList,
		ReplacementList,
		PPTokens,
		NumberLiteral,
		StringLiteral,
		CharacterLiteral,
		HeaderName,
		HeaderNameString,
		Empty,
	};

	enum class OverloadableOperatorType
	{
		None,
		New,
		Delete,
		NewArr,
		DeleteArr,
		Plus,
		Minus,
		Multiply,
		Divide,
		Modulo,
		Xor,
		BitAnd,
		BitOr,
		BitComplement,
		Not,
		Assign,
		LessThan,
		GreaterThan,
		PlusEqual,
		MinusEqual,
		MultiplyEqual,
		DivideEqual,
		ModuloEqual,
		CaretEqual,
		BitAndEqual,
		BitOrEqual,
		LeftShift,
		RightShift,
		RightShiftEqual,
		LeftShiftEqual,
		EqualEqual,
		NotEqual,
		LessThanEqual,
		GreaterThanEqual,
		LogicAnd,
		LogicOr,
		PlusPlus,
		MinusMinus,
		Comma,
		ArrowStar,
		Arrow,
		ParenGroup,
		AddressOf,
		Dereference,
		Negative,
		Positive,
		BracketGroup
	};
	enum class AssignmentOperatorType
	{
		None,
		Equal,
		TimesEqual,
		DivEqual,
		ModEqual,
		PlusEqual,
		MinusEqual,
		RightShiftEqual,
		LeftShiftEqual,
		AndEqual,
		XorEqual,
		OrEqual
	};
	enum class EnumKeyType
	{
		Enum,
		Class,
		Struct
	};
	enum class CastType
	{
		DynamicCast,
		ReinterpretCast,
		StaticCast,
		ConstCast,
	};
	enum class MemberOperatorType
	{
		DotOperator,
		ArrowOperator
	};
	enum class AutoFunctionType
	{
		Default,
		Delete
	};

	struct AlignmentExpressionNode
	{
		AstNode* typeId;
	};

	struct ElaboratedSpecifierClassNode
	{
		AstNode* classKey;
		AstNode* attributeSpecifierSeq;
		AstNode* nestedNameSpecifier;
		Token identifier;
		bool hasScopeOp;
	};

	struct ElaboratedSpecifierTemplateNode
	{
		AstNode* classKey;
		AstNode* nestedNameSpecifier;
		AstNode* simpleTemplateId;
		bool hasScopeOp;
		bool hasTemplateKeyword;
	};

	struct ElaboratedSpecifierEnumNode
	{
		AstNode* nestedNameSpecifier;
		Token identifier;
		bool hasScopeOp;
	};

	struct UnqualifiedIdDtorDecltypeNode
	{
		AstNode* decltypeSpecifier;
	};

	struct UnqualifiedIdDtorClassNode
	{
		AstNode* className;
	};

	struct UnqualifiedIdNode
	{
		Token identifier;
	};

	struct NoptrConstantDeclaratorNode
	{
		AstNode* noptrNewDeclarator;
		AstNode* constantExpression;
		AstNode* attributeSpecifierSeq;
	};

	struct NoptrDeclaratorNode
	{
		AstNode* expression;
		AstNode* attributeSpecifierSeq;
	};

	struct NoptrAbstractExpressionDeclaratorNode
	{
		AstNode* ptrAbstractDeclarator;
		AstNode* constantExpression;
		AstNode* attributeSpecifierSeq;
		AstNode* noptrAbstractDeclarator;
	};

	struct NoptrAbstractDeclaratorNode
	{
		AstNode* ptrAbstractDeclarator;
		AstNode* parametersAndQualifiers;
		AstNode* noptrAbstractDeclarator;
	};

	struct HeaderNameStringNode
	{
		Token stringLiteral;
	};

	struct HeaderNameNode
	{
		Token identifier;
	};

	struct CharacterLiteralNode
	{
		Token characterLiteral;
	};

	struct StringLiteralNode
	{
		Token stringLiteral;
	};

	struct NumberLiteralNode
	{
		Token numberLiteral;
	};

	struct PPTokensNode
	{
		AstNode* preprocessingToken;
		AstNode* nextPreprocessingToken;
	};

	struct ReplacementListNode
	{
		AstNode* ppTokens;
	};

	struct IdentifierListNode
	{
		AstNode* thisIdentifierNode;
		AstNode* nextIdentifierNode;
	};

	struct IdentifierNode
	{
		Token identifier;
	};

	struct NonDirectiveNode
	{
		AstNode* ppTokens;
	};

	struct TextLineNode
	{
		AstNode* ppTokens;
	};

	struct MacroPragmaNode
	{
		AstNode* ppTokens;
	};

	struct MacroErrorNode
	{
		AstNode* ppTokens;
	};

	struct MacroLineNode
	{
		AstNode* ppTokens;
	};

	struct MacroUndefNode
	{
		Token identifier;
	};

	struct MacroDefineFunctionNode
	{
		Token identifier;
		AstNode* identifierList;
		AstNode* replacementList;
	};

	struct MacroDefineNode
	{
		Token identifier;
		AstNode* replacementList;
	};

	struct MacroIncludeNode
	{
		AstNode* ppTokens;
	};

	struct ElseGroupNode
	{
		AstNode* group;
	};

	struct ElifGroupNode
	{
		AstNode* constantExpression;
		AstNode* group;
	};

	struct ElifGroupsNode
	{
		AstNode* thisElifGroup;
		AstNode* nextElifGroup;
	};

	struct IfNDefGroupNode
	{
		Token identifier;
		AstNode* group;
	};

	struct IfDefGroupNode
	{
		Token identifier;
		AstNode* group;
	};

	struct IfGroupNode
	{
		AstNode* constantExpression;
		AstNode* group;
	};

	struct IfSectionNode
	{
		AstNode* ifGroup;
		AstNode* elifGroups;
		AstNode* elseGroup;
	};

	struct GroupNode
	{
		AstNode* thisGroupPart;
		AstNode* nextGroupPart;
	};

	struct PreprocessingFileNode
	{
		AstNode* group;
	};

	struct NoexceptExpressionNode
	{
		AstNode* constantExpression;
	};

	struct TypeIdListNode
	{
		AstNode* thisTypeId;
		AstNode* nextTypeId;
	};

	struct DynamicExceptionSpecNode
	{
		AstNode* typeIdList;
	};

	struct ThrowExpressionNode
	{
		AstNode* assignmentExpression;
	};

	struct ExceptionAbstractDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* typeSpecifierSeq;
		AstNode* abstractDeclarator;
	};

	struct ExceptionDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* typeSpecifierSeq;
		AstNode* declarator;
	};

	struct HandlerNode
	{
		AstNode* exceptionDeclaration;
		AstNode* compoundStatement;
	};

	struct HandlerSeqNode
	{
		AstNode* thisHandler;
		AstNode* nextHandler;
	};

	struct FunctionTryBlockNode
	{
		AstNode* ctorInitializer;
		AstNode* compoundStatement;
		AstNode* handlerSeq;
	};

	struct TryBlockNode
	{
		AstNode* compoundStatement;
		AstNode* handlerSeq;
	};

	struct ExplicitInstantiationNode
	{
		AstNode* declaration;
		bool hasExternKeyword;
	};

	struct TypenameSpecifierNode
	{
		AstNode* nestedNameSpecifier;
		Token identifier;
	};

	struct TypenameTemplateSpecifierNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* simpleTemplateId;
		bool hasTemplateKeyword;
	};

	struct TemplateNameNode
	{
		Token identifier;
	};

	struct FunctionOperatorTemplateIdNode
	{
		AstNode* operatorFunctionId;
		AstNode* templateArgumentList;
	};

	struct LiteralOperatorTemplateIdNode
	{
		AstNode* literalOperatorId;
		AstNode* templateArgumentList;
	};

	struct SimpleTemplateIdNode
	{
		AstNode* templateName;
		AstNode* templateArgumentList;
	};

	struct TemplateParameterListNode
	{
		AstNode* thisParameter;
		AstNode* nextParameter;
	};

	struct TemplateDeclarationNode
	{
		AstNode* templateParameterList;
		AstNode* declaration;
	};

	struct LiteralOperatorIdNode
	{
		Token identifier;
	};

	struct FunctionBodyNode
	{
		AstNode* ctorInitializer;
		AstNode* compoundStatement;
	};

	struct FunctionDefinitionNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
		AstNode* functionBody;
	};

	struct FunctionDefaultDefinitionNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
		AutoFunctionType functionType;
	};

	struct ParameterAbstractDefaultDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* abstractDeclarator;
		AstNode* initializerClause;
	};

	struct ParameterAbstractDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* abstractDeclarator;
	};

	struct ParameterDefaultDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
		AstNode* initializerClause;
	};

	struct ParameterDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
	};

	struct ParameterDeclarationListNode
	{
		AstNode* thisParameter;
		AstNode* nextParameter;
	};

	struct PtrAbstractDeclaratorNode
	{
		AstNode* ptrOperator;
		AstNode* ptrAbstractDeclarator;
	};

	struct AbstractDeclaratorNode
	{
		AstNode* noptrAbstractDeclarator;
		AstNode* parametersAndQualifiers;
		AstNode* trailingReturnType;
	};

	struct DecltypeSpecNode
	{
		AstNode* expression;
	};

	struct SimpleTypeTokenSpecNode
	{
		Token type;
	};

	struct SimpleTypeTemplateSpecNode
	{
		AstNode* nestedNameSpec;
		AstNode* simpleTemplateId;
	};

	struct SimpleTypeSpecNode
	{
		AstNode* nestedNameSpec;
		AstNode* typeName;
	};

	struct TrailingTypeSpecSeqNode
	{
		AstNode* thisTypeSpec;
		AstNode* nextTypeSpec;
		AstNode* attributeSpecifierSeq;
	};

	struct TypeSpecSeqNode
	{
		AstNode* thisTypeSpec;
		AstNode* nextTypeSpec;
		AstNode* attributeSpecifierSeq;
	};

	struct TypedefNameNode
	{
		Token identifier;
	};

	struct FunctionSpecNode
	{
		Token specifier;
	};

	struct StorageClassSpecNode
	{
		Token specifier;
	};

	struct DeclSpecSeqNode
	{
		AstNode* thisSpec;
		AstNode* nextSpec;
		AstNode* attributeSpecifierSeq;
	};

	struct SimpleDeclSpecifierNode
	{
		Token token;
	};

	struct DeclSpecifierNode
	{
		AstNode* specifier;
	};

	struct StaticAssertDeclarationNode
	{
		AstNode* constantExpression;
		Token stringLiteral;
	};

	struct SimpleDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* initDeclaratorList;
	};

	struct AliasDeclarationNode
	{
		Token identifier;
		AstNode* typeId;
	};

	struct DeclarationSeqNode
	{
		AstNode* thisDeclaration;
		AstNode* nextDeclaration;
	};

	struct NewInitializerNode
	{
		AstNode* expressionList;
	};

	struct NoptrNewTailDeclaratorNode
	{
		AstNode* expression;
		AstNode* attributeSpecifierSeq;
	};

	struct NoptrNewDeclaratorNode
	{
		AstNode* noptrNewDeclarator;
		AstNode* constantExpression;
		AstNode* attributeSpecifierSeq;
	};

	struct NewDeclaratorNode
	{
		AstNode* ptrOperator;
		AstNode* newDeclarator;
	};

	struct NewTypeIdNode
	{
		AstNode* typeSpecifierSeq;
		AstNode* newDeclarator;
	};

	struct NewPlacementNode
	{
		AstNode* expressionList;
	};

	struct NewExpressionNode
	{
		AstNode* newPlacement;
		AstNode* typeId;
		AstNode* newInitializer;
	};

	struct NewTypeIdExpressionNode
	{
		AstNode* newPlacement;
		AstNode* newTypeId;
		AstNode* newInitializer;
	};

	struct PseudoDestructorNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* typeName;
	};

	struct PseudoNestedDestructorNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* nestedTypeName;
		AstNode* typeName;
	};

	struct PseudoDestructorTemplateNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* simpleTemplateId;
		AstNode* typeName;
	};

	struct PseudoDestructorDecltypeNode
	{
		AstNode* decltypeSpecifier;
	};

	struct PostfixPlusPlusNode
	{
		AstNode* postfixExpression;
	};

	struct PostfixMinusMinusNode
	{
		AstNode* postfixExpression;
	};

	struct PostfixPseudoDestructorNode
	{
		AstNode* postfixExpression;
		AstNode* pseudoDestructorName;
		MemberOperatorType memberOp;
	};

	struct PostfixMemberIdExpressionNode
	{
		AstNode* postfixExpression;
		AstNode* idExpression;
		bool hasTemplateKeyword;
		MemberOperatorType memberOp;
	};

	struct PostfixParenExpressionListNode
	{
		AstNode* postfixExpression;
		AstNode* expressionList;
	};

	struct PostfixBracketBraceListNode
	{
		AstNode* postfixExpression;
		AstNode* bracedInitList;
	};

	struct PostfixBracketExpressionNode
	{
		AstNode* postfixExpression;
		AstNode* expression;
	};

	struct PostfixTypeIdExpressionNode
	{
		AstNode* expression;
	};

	struct PostfixTypeIdNode
	{
		AstNode* typeId;
	};

	struct PostfixCastNode
	{
		AstNode* typeId;
		AstNode* expression;
		CastType castType;
	};

	struct PostfixTypenameSpecExpressionListNode
	{
		AstNode* typenameSpecifier;
		AstNode* expressionList;
	};

	struct PostfixTypenameSpecBraceListNode
	{
		AstNode* typenameSpecifier;
		AstNode* bracedInitList;
	};

	struct PostfixSimpleTypeExpressionListNode
	{
		AstNode* simpleTypeSpecifier;
		AstNode* expressionList;
	};

	struct PostfixSimpleTypeBraceListNode
	{
		AstNode* simpleTypeSpecifier;
		AstNode* bracedInitList;
	};

	struct NestedNamespaceSpecifierTemplateNode
	{
		AstNode* nestedNameSpecifier;
		bool hasTemplateKeyword;
		AstNode* simpleTemplateId;
	};

	struct NestedNamespaceSpecifierIdNode
	{
		AstNode* nestedNameSpecifier;
		Token identifier;
	};

	struct MemInitializerIdNode
	{
		Token identifier;
	};

	struct MemBracedInitializerNode
	{
		AstNode* memInitializerId;
		AstNode* bracedInitList;
	};

	struct MemExpressionInitializerNode
	{
		AstNode* memInitializerId;
		AstNode* expressionList;
	};

	struct MemInitializerListNode
	{
		AstNode* thisMemInitializer;
		AstNode* nextMemInitializer;
	};

	struct ConversionDeclaratorNode
	{
		AstNode* ptrOperator;
		AstNode* conversionDeclarator;
	};

	struct ConversionTypeIdNode
	{
		AstNode* typeSpecifierSeq;
		AstNode* conversionDeclarator;
	};

	struct ConversionFunctionIdNode
	{
		AstNode* conversionTypeId;
	};

	struct AccessSpecifierNode
	{
		Token accessSpecifier;
	};

	struct ClassOrDecltypeNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* className;
	};

	struct BaseSpecifierNode
	{
		AstNode* attributeSpecifierSeq;
		bool isVirtual;
		AstNode* accessSpecifier;
		AstNode* baseTypeSpecifier;
	};

	struct BaseSpecifierListNode
	{
		AstNode* thisBaseSpecifier;
		AstNode* nextBaseSpecifier;
	};

	struct VirtSpecifierNode
	{
		Token token;
	};

	struct VirtSpecifierSeqNode
	{
		AstNode* thisSpec;
		AstNode* nextSpec;
	};

	struct MemberDeclaratorNode
	{
		Token identifier;
		AstNode* attributeSpecifierSeq;
		AstNode* virtSpecifierSeq;
		AstNode* constantExpression;
	};

	struct MemberDeclaratorBraceNode
	{
		AstNode* declarator;
		AstNode* virtSpecifierSeq;
		AstNode* braceOrEqualInitializer;
	};

	struct MemberDeclaratorPureNode
	{
		AstNode* declarator;
		AstNode* virtSpecifierSeq;
		AstNode* pureSpecifier;
	};

	struct MemberDeclaratorListNode
	{
		AstNode* thisDeclarator;
		AstNode* nextDeclarator;
	};

	struct MemberDeclarationNode
	{
		AstNode* attribtueSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* memberDeclaratorList;
	};

	struct MemberFunctionDeclarationNode
	{
		AstNode* functionDefinition;
		bool hasTrailingSemicolon;
	};

	struct MemberSpecifierNode
	{
		AstNode* memberDeclaration;
		AstNode* memberSpecification;
	};

	struct MemberAndAccessSpecifierNode
	{
		AstNode* accessSpecifier;
		AstNode* memberSpecification;
	};

	struct ClassKeyNode
	{
		Token token;
	};

	struct ClassVirtSpecifierNode
	{
		Token token;
	};

	struct ClassVirtSpecifierSeqNode
	{
		AstNode* thisSpec;
		AstNode* nextSpec;
	};

	struct ClassHeadNameNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* className;
	};

	struct ClassVirtualHeadNode
	{
		AstNode* classKey;
		AstNode* attributeSpecifierSeq;
		AstNode* classHeadName;
		AstNode* classVirtSpecifierSeq;
		AstNode* baseClause;
	};

	struct ClassHeadNode
	{
		AstNode* classKey;
		AstNode* attributeSpecifierSeq;
		AstNode* baseClause;
	};

	struct ClassSpecifierNode
	{
		AstNode* classHead;
		AstNode* memberSpecification;
	};

	struct ClassNameNode
	{
		Token identifier;
	};

	struct DeclaratorIdNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* className;
	};

	struct RefQualifierNode
	{
		bool doubleRef;
	};

	struct CvQualifierNode
	{
		Token qualifier;
	};

	struct CvQualifierSeqNode
	{
		AstNode* thisQualifier;
		AstNode* nextQualifier;
	};

	struct PtrStarNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* cvQualifierSeq;
	};

	struct RefNode
	{
		AstNode* attributeSpecifierSeq;
	};

	struct RefRefNode
	{
		AstNode* attributeSpecifierSeq;
	};

	struct PtrNamespaceStarNode
	{
		AstNode* nestedNameSpecifier;
		AstNode* attributeSpecifierSeq;
		AstNode* cvQualifierSeq;
	};

	struct TrailingReturnTypeNode
	{
		AstNode* trailingTypeSpecifierSeq;
		AstNode* abstractDeclarator;
	};

	struct ParametersAndQualifiersNode
	{
		AstNode* parameterDeclarationClause;
		AstNode* attributeSpecifierSeq;
		AstNode* cvQualifierSeq;
		AstNode* refQualifier;
		AstNode* exceptionSpecification;
	};

	struct PtrDeclaratorNode
	{
		AstNode* ptrOperator;
		AstNode* ptrDeclarator;
	};

	struct DeclaratorNode
	{
		AstNode* noPtrDeclarator;
		AstNode* parametersAndQualifiers;
		AstNode* trailingReturnType;
	};

	struct InitDeclaratorNode
	{
		AstNode* declarator;
		AstNode* initializer;
	};

	struct InitDeclaratorListNode
	{
		AstNode* thisDeclarator;
		AstNode* nextDeclarator;
	};

	struct BalancedTokenNode
	{
		Token token;
	};

	struct BalancedTokenSeqNode
	{
		AstNode* thisToken;
		AstNode* nextToken;
	};

	struct AttributeArgumentClauseNode
	{
		AstNode* balancedTokenSequence;
	};

	struct AttributeTokenNode
	{
		Token namespaceName;
		Token identifier;
	};

	struct AttributeNode
	{
		AstNode* attributeToken;
		AstNode* attributeArgumentClause;
	};

	struct AttributeListNode
	{
		AstNode* thisAttribute;
		AstNode* nextAttribute;
	};

	struct AlignAsTypeIdNode
	{
		AstNode* typeId;
		bool hasElipsis;
	};

	struct AlignAsExpressionNode
	{
		AstNode* alignmentExpression;
		bool hasElipsis;
	};

	struct AttributeSpecifierSequenceNode
	{
		AstNode* thisSpec;
		AstNode* nextSpec;
	};

	struct LinkageSpecificationBlockNode
	{
		Token stringLiteral;
		AstNode* declarationSeq;
	};

	struct LinkageSpecificationNode
	{
		Token stringLiteral;
		AstNode* declaration;
	};

	struct AsmNode
	{
		Token stringLiteral;
	};

	struct UsingDirectiveNode
	{
		AstNode* attributeSpecifierSeq;
		bool isNested;
		AstNode* nestedNameSpecifier;
		AstNode* namespaceName;
	};

	struct UsingDeclarationNode
	{
		AstNode* unqualifiedId;
	};

	struct UsingTypenameDeclarationNode
	{
		bool hasTypename;
		bool isNested;
		AstNode* nestedNameSpecifier;
		AstNode* unqualifiedId;
	};

	struct QualifiedNamespaceSpecifierNode
	{
		bool isNested;
		AstNode* nestedNameSpecifier;
		AstNode* namespaceName;
	};

	struct NamespaceAliasDefinitionNode
	{
		Token identifier;
		AstNode* qualifiedNamespaceSpecifier;
	};

	struct UnnamedNamespaceDefinitionNode
	{
		bool isInline;
		AstNode* namespaceBody;
	};

	struct NamedNamespaceDefinitionNode
	{
		bool isInline;
		Token identifier;
		AstNode* namespaceBody;
	};

	struct NamespaceNameNode
	{
		Token identifier;
	};

	struct ReturnNode
	{
		AstNode* returnValue;
	};

	struct GotoNode
	{
		Token identifier;
	};

	struct StatementSequenceNode
	{
		AstNode* thisStatement;
		AstNode* nextStatement;
	};

	struct CompoundStatementNode
	{
		AstNode* statementSequence;
	};

	struct LabeledIdentifierNode
	{
		AstNode* attributeSpecifierSeq;
		Token identifier;
		AstNode* statement;
	};

	struct CaseLabelNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* constantExpression;
		AstNode* statement;
	};

	struct DefaultLabelNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* statement;
	};

	struct QualifiedIdNode
	{
		Token identifier;
	};

	struct TemplateQualifiedIdNode
	{
		AstNode* nestedNamespaceSpecifier;
		bool hasNamespaceScope;
		bool hasTemplateKeyword;
	};

	struct StatementNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* statement;
	};

	struct ForRangeInitializerNode
	{
		AstNode* expression;
		AstNode* bracedInitList;
	};

	struct ForRangeDeclarationNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* typeSpecifierSeq;
		AstNode* declarator;
	};

	struct WhileLoopNode
	{
		AstNode* condition;
		AstNode* statement;
	};

	struct DoWhileLoopNode
	{
		AstNode* statement;
		AstNode* condition;
	};

	struct ForLoopNode
	{
		AstNode* forInitStatement;
		AstNode* condition;
		AstNode* expression;
		AstNode* statement;
	};

	struct ForEachLoopNode
	{
		AstNode* forRangeDeclaration;
		AstNode* forRangeInitializer;
		AstNode* statement;
	};

	struct InitializerConditionNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
		AstNode* initializerClause;
	};

	struct BracedInitConditionNode
	{
		AstNode* attributeSpecifierSeq;
		AstNode* declSpecifierSeq;
		AstNode* declarator;
		AstNode* bracedInitList;
	};

	struct IfElseNode
	{
		AstNode* condition;
		AstNode* ifStatement;
		AstNode* elseStatement;
	};

	struct SwitchNode
	{
		AstNode* condition;
		AstNode* statement;
	};

	struct ConstantExpressionNode
	{
		AstNode* expression;
	};

	struct EnumeratorDefinitionNode
	{
		Token identifier;
		AstNode* value;
	};

	struct EnumeratorListNode
	{
		AstNode* enumDefinition;
		AstNode* nextEnumDefinition;
	};

	struct EnumBaseNode
	{
		AstNode* TypeSpecifierSeq;
	};

	struct OpaqueEnumDeclNode
	{
		AstNode* enumKey;
		AstNode* attributeSpecifierSeq;
		Token identifier;
		AstNode* enumBase;
	};

	struct EnumHeadNode
	{
		AstNode* enumKey;
		AstNode* attributeSpecifierSeq;
		AstNode* nestedNameSpecifier;
		Token identifier;
		AstNode* enumBase;
	};

	struct EnumNameNode
	{
		Token identifier;
	};

	struct EnumSpecifierNode
	{
		AstNode* enumHead;
		AstNode* enumeratorList;
	};

	struct EnumKeyNode
	{
		EnumKeyType type;
	};

	struct TypeIdNode
	{
		AstNode* typeSpecifierSeq;
		AstNode* abstractDeclarator;
	};

	struct TemplateArgumentListNode
	{
		AstNode* thisArgument;
		AstNode* nextArgument;
	};

	struct LambdaExpressionNode
	{
		AstNode* introducer;
		AstNode* declarator;
		AstNode* compoundStatement;
	};

	struct LambdaIntroducerNode
	{
		AstNode* lambdaCapture;
	};

	struct LambdaCaptureNode
	{
		AstNode* captureList;
		bool hasDefaultRef;
		bool hasDefaultCopy;
	};

	struct CaptureNode
	{
		Token identifier;
	};

	struct LambdaCaptureListNode
	{
		AstNode* thisCapture;
		AstNode* nextCapture;
	};

	struct LambdaDeclaratorNode
	{
		AstNode* parameterDeclarationClause;
		AstNode* exceptionSpecification;
		AstNode* attributeSpecifierSequence;
		AstNode* trailingReturnType;
		bool isMutable;
	};

	struct GroupingNode
	{
		AstNode* expression;
	};

	struct LiteralNode
	{
		Token token;
	};

	struct ThisNode
	{
		Token token;
	};

	struct DeleteNode
	{
		AstNode* expression;
		bool deleteArray;
	};

	struct SizeofExpressionNode
	{
		AstNode* expression;
	};

	struct SizeofIdentifierExpressionNode
	{
		Token identifier;
	};

	struct AlignofExpressionNode
	{
		AstNode* expression;
	};

	struct UnaryExpressionNode
	{
		OverloadableOperatorType opType;
		AstNode* expression;
	};

	struct CastExpressionNode
	{
		AstNode* typeId;
		AstNode* expression;
	};

	struct PointerToMemberNode
	{
		AstNode* left;
		AstNode* right;
	};

	struct ExpressionNode
	{
		AstNode* expression;
		AstNode* nextExpression;
	};

	struct AssignmentExpressionNode
	{
		AstNode* leftSide;
		AssignmentOperatorType opType;
		AstNode* initializerClause;
	};

	struct TernaryExpressionNode
	{
		AstNode* comparisonExpression;
		AstNode* ifTrueNode;
		AstNode* ifFalseNode;
	};

	struct TypeParameterNode
	{
		AstNode* templateParameterList;
		AstNode* typeId;
		Token identifier;
	};

	struct BinaryExpressionNode
	{
		AstNode* left;
		AstNode* right;
		OverloadableOperatorType opType;
	};

	struct OperatorFunctionIdNode
	{
		OverloadableOperatorType opType;
		AstNode* templateArgList;
	};

	struct UsingAliasNode
	{
		AstNode* typeIdNode;
	};

	struct BracedInitListNode
	{
		AstNode* initList;
	};

	struct InitializerClauseNode
	{
		AstNode* clause;
	};

	struct InitializerListNode
	{
		std::vector<AstNode*> clauses;
	};

	struct AstNode
	{
		~AstNode() = delete;

		AstNodeType type;
		bool success;
		union
		{
			UsingAliasNode usingAliasNode;
			BracedInitListNode bracedInitList;
			InitializerClauseNode initializerClauseNode;
			InitializerListNode initializerList;
			OperatorFunctionIdNode operatorFunctionId;
			TypeParameterNode typeParameter;
			BinaryExpressionNode binaryExpression;
			TernaryExpressionNode ternaryExpression;
			AssignmentExpressionNode assignmentExpression;
			ExpressionNode expressionNode;
			PointerToMemberNode pointerToMember;
			CastExpressionNode castExpression;
			UnaryExpressionNode unaryExpression;
			SizeofExpressionNode sizeofExpression;
			SizeofIdentifierExpressionNode sizeofIdentifierExpression;
			AlignofExpressionNode alignofExpression;
			DeleteNode deleteNode;
			LiteralNode literalNode;
			ThisNode thisNode;
			GroupingNode grouping;
			LambdaExpressionNode lambdaExpression;
			LambdaIntroducerNode lambdaIntroducer;
			LambdaCaptureNode lambdaCapture;
			CaptureNode capture;
			LambdaCaptureListNode lambdaCaptureList;
			LambdaDeclaratorNode lambdaDeclarator;
			TemplateArgumentListNode templateArgumentList;
			TemplateQualifiedIdNode templateQualifiedId;
			TypeIdNode typeIdNode;
			EnumNameNode enumName;
			EnumSpecifierNode enumSpecifier;
			EnumKeyNode enumKey;
			EnumHeadNode enumHead;
			OpaqueEnumDeclNode opaqueEnumDecl;
			EnumBaseNode enumBase;
			EnumeratorListNode enumeratorList;
			EnumeratorDefinitionNode enumeratorDefinition;
			ConstantExpressionNode constantExpression;
			IfElseNode ifElseNode;
			SwitchNode switchNode;
			InitializerConditionNode initCondition;
			BracedInitConditionNode bracedInitCondition;
			WhileLoopNode whileLoopNode;
			DoWhileLoopNode doWhileLoopNode;
			ForLoopNode forLoopNode;
			ForEachLoopNode forEachLoopNode;
			ForRangeDeclarationNode forRangeDeclaration;
			ForRangeInitializerNode forRangeInitializer;
			StatementNode statement;
			QualifiedIdNode qualifeidId;
			LabeledIdentifierNode labeledIdentifier;
			CaseLabelNode caseLabel;
			DefaultLabelNode defaultLabel;
			CompoundStatementNode compoundStatement;
			StatementSequenceNode statementSequence;
			ReturnNode returnNode;
			GotoNode gotoNode;
			NamespaceNameNode namespaceNameNode;
			NamedNamespaceDefinitionNode namedNamespaceDefinition;
			UnnamedNamespaceDefinitionNode unnamedNamespaceDefinition;
			NamespaceAliasDefinitionNode namespaceAliasDefinition;
			QualifiedNamespaceSpecifierNode qualifiedNamespaceSpecifier;
			UsingDeclarationNode usingDeclaration;
			UsingTypenameDeclarationNode usingTypenameDeclaration;
			UsingDirectiveNode usingDirective;
			AsmNode asmNode;
			LinkageSpecificationBlockNode linkageSpecificationBlock;
			LinkageSpecificationNode linkageSpecification;
			AttributeSpecifierSequenceNode attributeSpecifierSeq;
			AlignAsTypeIdNode alignAsTypeId;
			AlignAsExpressionNode alignAsExpression;
			AttributeListNode attributeList;
			AttributeNode attribute;
			AttributeTokenNode attributeToken;
			AttributeArgumentClauseNode attributeArgumentClause;
			BalancedTokenSeqNode balancedTokenSeq;
			BalancedTokenNode balancedToken;
			InitDeclaratorListNode initDeclaratorList;
			InitDeclaratorNode initDeclarator;
			DeclaratorNode declaratorNode;
			PtrDeclaratorNode ptrDeclarator;
			ParametersAndQualifiersNode parametersAndQualifiersNode;
			TrailingReturnTypeNode trailingReturnTypeNode;
			PtrStarNode ptrStar;
			RefNode ref;
			RefRefNode refRef;
			PtrNamespaceStarNode ptrNamespaceStar;
			CvQualifierSeqNode cvQualifierSeq;
			CvQualifierNode cvQualifier;
			RefQualifierNode refQualifier;
			DeclaratorIdNode declaratorId;
			ClassNameNode className;
			ClassSpecifierNode classSpecifier;
			ClassVirtualHeadNode classVirtualHead;
			ClassHeadNode classHead;
			ClassHeadNameNode classHeadName;
			ClassVirtSpecifierSeqNode classVirtSpecifierSeq;
			ClassVirtSpecifierNode classVirtSpecifier;
			ClassKeyNode classKey;
			MemberAndAccessSpecifierNode memberAndAccessSpecifier;
			MemberSpecifierNode memberSpecifier;
			MemberFunctionDeclarationNode memberFunctionDeclaration;
			MemberDeclarationNode memberDeclarationNode;
			MemberDeclaratorListNode memberDeclaratorList;
			MemberDeclaratorPureNode memberDeclaratorPure;
			MemberDeclaratorBraceNode memberDeclaratorBrace;
			MemberDeclaratorNode memberDeclarator;
			VirtSpecifierSeqNode virtSpecifierSeq;
			VirtSpecifierNode virtSpecifier;
			BaseSpecifierListNode baseSpecifierList;
			BaseSpecifierNode baseSpecifier;
			ClassOrDecltypeNode classOrDecltype;
			AccessSpecifierNode accessSpecifier;
			ConversionFunctionIdNode conversionFunctionId;
			ConversionTypeIdNode conversionTypeId;
			ConversionDeclaratorNode conversionDeclarator;
			MemInitializerListNode memInitializerList;
			MemExpressionInitializerNode memExpressionInitializer;
			MemBracedInitializerNode memBracedInitializer;
			MemInitializerIdNode memInitializerId;
			NestedNamespaceSpecifierIdNode nestedNamespaceSpecifierId;
			NestedNamespaceSpecifierTemplateNode nestedNamespaceSpecifierTemplate;
			PostfixSimpleTypeBraceListNode postfixSimpleTypeBraceList;
			PostfixSimpleTypeExpressionListNode postfixSimpleTypeExpressionList;
			PostfixTypenameSpecExpressionListNode postfixTypenameSpecExpressionList;
			PostfixTypenameSpecBraceListNode postfixTypenameSpecBraceList;
			PostfixCastNode postfixCast;
			PostfixTypeIdExpressionNode postfixTypeIdExpression;
			PostfixTypeIdNode postfixTypeId;
			PostfixBracketExpressionNode postfixBracketExpression;
			PostfixBracketBraceListNode postfixBracketBraceList;
			PostfixParenExpressionListNode postfixParenExpressionList;
			PostfixMemberIdExpressionNode postfixMemberIdExpression;
			PostfixPseudoDestructorNode postfixPseudoDestructor;
			PostfixPlusPlusNode postfixPlusPlus;
			PostfixMinusMinusNode postfixMinusMinus;
			PseudoDestructorDecltypeNode pseudoDestructorDecltype;
			PseudoDestructorTemplateNode pseudoDestructorTemplate;
			PseudoDestructorNode pseudoDestructor;
			PseudoNestedDestructorNode pseudoNestedDestructor;
			NewTypeIdExpressionNode newTypeIdExpression;
			NewExpressionNode newExpression;
			NewPlacementNode newPlacementNode;
			NewTypeIdNode newTypeId;
			NewDeclaratorNode newDeclarator;
			NoptrNewTailDeclaratorNode noptrNewTailDeclarator;
			NoptrNewDeclaratorNode noptrNewDeclarator;
			NewInitializerNode newInitializer;
			DeclarationSeqNode declarationSeq;
			AliasDeclarationNode aliasDeclaration;
			SimpleDeclarationNode simpleDeclaration;
			StaticAssertDeclarationNode staticAssertDeclaration;
			SimpleDeclSpecifierNode simpleDeclSpecifier;
			DeclSpecifierNode declSpecifier;
			DeclSpecSeqNode declSpecSeq;
			StorageClassSpecNode storageClassSpec;
			FunctionSpecNode functionSpec;
			TypedefNameNode typedefName;
			TypeSpecSeqNode typeSpecSeq;
			TrailingTypeSpecSeqNode trailingTypeSpecSeq;
			SimpleTypeTokenSpecNode simpleTypeTokenSpec;
			SimpleTypeTemplateSpecNode simpleTypeTemplateSpec;
			SimpleTypeSpecNode simpleTypeSpec;
			DecltypeSpecNode decltypeSpec;
			AbstractDeclaratorNode abstractDeclarator;
			PtrAbstractDeclaratorNode ptrAbstractDeclarator;
			ParameterDeclarationListNode parameterDeclarationList;
			ParameterDeclarationNode parameterDeclaration;
			ParameterDefaultDeclarationNode parameterDefaultDeclaration;
			ParameterAbstractDeclarationNode parameterAbstractDeclaration;
			ParameterAbstractDefaultDeclarationNode parameterAbstractDefaultDeclaration;
			FunctionDefaultDefinitionNode functionDefaultDefinition;
			FunctionDefinitionNode functionDefinition;
			FunctionBodyNode functionBody;
			LiteralOperatorIdNode literalOperatorId;
			TemplateDeclarationNode templateDeclaration;
			TemplateParameterListNode templateParameterList;
			SimpleTemplateIdNode simpleTemplateId;
			FunctionOperatorTemplateIdNode functionOperatorTemplateId;
			LiteralOperatorTemplateIdNode literalOperatorTemplateId;
			TemplateNameNode templateName;
			TypenameSpecifierNode typenameSpecifier;
			TypenameTemplateSpecifierNode typenameTemplateSpecifier;
			ExplicitInstantiationNode explicitInstantiation;
			TryBlockNode tryBlock;
			FunctionTryBlockNode functionTryBlock;
			HandlerSeqNode handlerSeq;
			HandlerNode handler;
			ExceptionDeclarationNode exceptionDeclaration;
			ExceptionAbstractDeclarationNode exceptionAbstractDeclaration;
			ThrowExpressionNode throwExpression;
			DynamicExceptionSpecNode dynamicExceptionSpec;
			TypeIdListNode typeIdList;
			NoexceptExpressionNode noexceptExpression;
			NoptrAbstractDeclaratorNode noptrAbstractDeclarator;
			NoptrAbstractExpressionDeclaratorNode noptrAbstractExpressionDeclarator;
			NoptrDeclaratorNode noptrDeclarator;
			NoptrConstantDeclaratorNode noptrConstantDeclarator;
			UnqualifiedIdNode unqualifiedId;
			UnqualifiedIdDtorClassNode unqualifiedIdDtorClass;
			UnqualifiedIdDtorDecltypeNode unqualifiedIdDtorDecltype;
			ElaboratedSpecifierEnumNode elaboratedSpecifierEnum;
			ElaboratedSpecifierTemplateNode elaboratedSpecifierTemplate;
			ElaboratedSpecifierClassNode elaboratedSpecifierClass;
			AlignmentExpressionNode alignmentExpression;

			// Preprocessing Stuff
			PreprocessingFileNode preprocessingFile;
			GroupNode group;
			IfSectionNode ifSection;
			IfGroupNode ifGroup;
			IfDefGroupNode ifDefGroup;
			IfNDefGroupNode ifNDefGroup;
			ElifGroupsNode elifGroups;
			ElifGroupNode elifGroup;
			ElseGroupNode elseGroup;
			MacroIncludeNode macroInclude;
			MacroDefineNode macroDefine;
			MacroDefineFunctionNode macroDefineFunction;
			MacroUndefNode macroUndef;
			MacroLineNode macroLine;
			MacroErrorNode macroError;
			MacroPragmaNode macroPragma;
			TextLineNode textLine;
			NonDirectiveNode nonDirective;
			IdentifierListNode identifierList;
			IdentifierNode identifier;
			ReplacementListNode replacementList;
			PPTokensNode ppTokens;
			StringLiteralNode stringLiteral;
			NumberLiteralNode numberLiteral;
			CharacterLiteralNode characterLiteral;
			HeaderNameNode headerName;
			HeaderNameStringNode headerNameString;
		};
	};

	ScriptParser::ScriptParser(std::vector<Token>& tokens, CPath& fullFilepath)
		: m_Tokens(tokens), m_FullFilepath(fullFilepath)
	{
		Tokens = tokens;
	}

	static void ErrorAtToken()
	{
		Token& currentToken = Tokens[CurrentToken];
		Log::Error("Unexpected token '%s' at line %d:%d", ScriptScanner::TokenName(currentToken.m_Type).c_str(), currentToken.m_Line, currentToken.m_Column);
	}

	static void Consume(TokenType type)
	{
		Token& currentToken = Tokens[CurrentToken];
		if (currentToken.m_Type == type)
		{
			CurrentToken++;
			return;
		}

		Log::Error("Unexpected token. Expected '%s' instead got '%s'", ScriptScanner::TokenName(type).c_str(), ScriptScanner::TokenName(currentToken.m_Type).c_str());
	}

	static void BacktrackTo(int position)
	{
		Log::Assert(position >= 0 && position < Tokens.size(), "Invalid backtrack location.");
		CurrentToken = position;
	}

	static bool Match(TokenType type)
	{
		Token& currentToken = Tokens[CurrentToken];
		if (currentToken.m_Type == type)
		{
			CurrentToken++;
			return true;
		}

		return false;
	}

	static Token ConsumeCurrent(TokenType type)
	{
		Token& currentToken = Tokens[CurrentToken];
		if (currentToken.m_Type == type)
		{
			CurrentToken++;
			return currentToken;
		}

		Log::Error("Unexpected token. Expected '%s' instead got '%s'", ScriptScanner::TokenName(type).c_str(), ScriptScanner::TokenName(currentToken.m_Type).c_str());
		return currentToken;
	}

	static Token GetCurrentToken()
	{
		return Tokens[CurrentToken];
	}

	static bool IsAssignmentOperator(TokenType type)
	{
		return type == TokenType::EQUAL || type == TokenType::STAR_EQUAL || type == TokenType::DIV_EQUAL || type == TokenType::MODULO_EQUAL ||
			type == TokenType::PLUS_EQUAL || type == TokenType::MINUS_EQUAL || type == TokenType::RIGHT_SHIFT_EQUAL || type == TokenType::LEFT_SHIFT_EQUAL ||
			type == TokenType::AND_EQUAL || type == TokenType::CARET_EQUAL || type == TokenType::PIPE_EQUAL;
	}

	static void FreeNode(AstNode* node)
	{
		// TODO: Implement me by implementing a walk tree function
	}

	static AstNode* GenerateAstNode(AstNodeType type)
	{
		AstNode* node = (AstNode*)AllocMem(sizeof(AstNode));
		node->success = true;
		node->type = type;
		return node;
	}

	static AstNode* GenerateEmptyStatementNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::Empty);
		return result;
	}

	static AstNode* GenerateBinaryExpressionNode(AstNode* left, OverloadableOperatorType op, AstNode* right)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BinaryExpression);
		result->binaryExpression.left = left;
		result->binaryExpression.opType = op;
		result->binaryExpression.right = right;
		return result;
	}

	static AstNode* GenerateTernaryExpressionNode(AstNode* comparisonExpression, AstNode* ifTrueNode, AstNode* ifFalseNode)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TernaryExpression);
		result->ternaryExpression.comparisonExpression = comparisonExpression;
		result->ternaryExpression.ifTrueNode = ifTrueNode;
		result->ternaryExpression.ifFalseNode = ifFalseNode;
		return result;
	}

	static AstNode* GenerateAssignmentExpressionNode(AstNode* leftSide, AssignmentOperatorType opType, AstNode* initializerClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AssignmentExpression);
		result->assignmentExpression.leftSide = leftSide;
		result->assignmentExpression.opType = opType;
		result->assignmentExpression.initializerClause = initializerClause;
		return result;
	}

	static AstNode* GeneratePointerToMemberNode(AstNode* left, AstNode* right)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PointerToMember);
		result->pointerToMember.left = left;
		result->pointerToMember.right = right;
		return result;
	}

	static AstNode* GenerateCastExpressionNode(AstNode* typeId, AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CastExpression);
		result->castExpression.typeId = typeId;
		result->castExpression.expression = expression;
		return result;
	}

	static AstNode* GenerateUnaryExpressionNode(OverloadableOperatorType opType, AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UnaryExpression);
		result->unaryExpression.opType = opType;
		result->unaryExpression.expression = expression;
		return result;
	}

	static AstNode* GenerateSizeofExpressionNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SizeofExpression);
		result->sizeofExpression.expression = expression;
		return result;
	}

	static AstNode* GenerateSizeofIdentifierExpressionNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SizeofIdentifierExpression);
		result->sizeofIdentifierExpression.identifier = identifier;
		return result;
	}

	static AstNode* GenerateAlignofExpressionNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AlignofExpression);
		result->alignofExpression.expression = expression;
		return result;
	}

	static AstNode* GenerateDeleteNode(AstNode* expression, bool deleteArray)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Delete);
		result->deleteNode.expression = expression;
		result->deleteNode.deleteArray = deleteArray;
		return result;
	}

	static AstNode* GenerateLiteralNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Literal);
		result->literalNode.token = token;
		return result;
	}

	static AstNode* GenerateThisNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::This);
		result->thisNode.token = token;
		return result;
	}

	static AstNode* GenerateGroupingNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Grouping);
		result->grouping.expression = expression;
		return result;
	}

	static AstNode* GenerateLambdaExpressionNode(AstNode* introducer, AstNode* declarator, AstNode* compoundStatement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LambdaExpression);
		result->lambdaExpression.introducer = introducer;
		result->lambdaExpression.declarator = declarator;
		result->lambdaExpression.compoundStatement = compoundStatement;
		return result;
	}

	static AstNode* GenerateLambdaIntroducerNode(AstNode* capture)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LambdaIntroducer);
		result->lambdaIntroducer.lambdaCapture = capture;
		return result;
	}

	static AstNode* GenerateLambdaCaptureNode(Token captureDefault, AstNode* captureList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LambdaCapture);
		result->lambdaCapture.hasDefaultRef = captureDefault.m_Type == TokenType::AND;
		result->lambdaCapture.hasDefaultCopy = captureDefault.m_Type == TokenType::EQUAL;
		result->lambdaCapture.captureList = captureList;
		return result;
	}

	static AstNode* GenerateCaptureNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Capture);
		result->capture.identifier = identifier;
		return result;
	}

	static AstNode* GenerateLambdaCaptureListNode(AstNode* thisCapture, AstNode* nextCapture)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LambdaCaptureList);
		result->lambdaCaptureList.thisCapture = thisCapture;
		result->lambdaCaptureList.nextCapture = nextCapture;
		return result;
	}

	static AstNode* GenerateLambdaDeclaratorNode(AstNode* parameterDeclarationClause, bool isMutable, AstNode* exceptionSpec, AstNode* attributeSpec, AstNode* trailingReturnType)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LambdaDeclarator);
		result->lambdaDeclarator.parameterDeclarationClause = parameterDeclarationClause;
		result->lambdaDeclarator.exceptionSpecification = exceptionSpec;
		result->lambdaDeclarator.attributeSpecifierSequence = attributeSpec;
		result->lambdaDeclarator.trailingReturnType = trailingReturnType;
		result->lambdaDeclarator.isMutable = isMutable;
		return result;
	}

	static AstNode* GenerateOperatorFunctionIdNode(OverloadableOperatorType opType, AstNode* templateArgList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::OperatorFunctionId);
		result->operatorFunctionId.opType = opType;
		result->operatorFunctionId.templateArgList = templateArgList;
		return result;
	}

	static AstNode* GenerateTemplateArgumentListNode(AstNode* thisArgument, AstNode* nextArgument)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TemplateArgumentList);
		result->templateArgumentList.thisArgument = thisArgument;
		result->templateArgumentList.nextArgument = nextArgument;
		return result;
	}

	static AstNode* GenerateTemplateQualifiedIdNode(AstNode* nestedNamespaceSpecifier, bool hasNamespaceScope, bool hasTemplateKeyword)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TemplateQualifiedId);
		result->templateQualifiedId.nestedNamespaceSpecifier = nestedNamespaceSpecifier;
		result->templateQualifiedId.hasNamespaceScope = hasNamespaceScope;
		result->templateQualifiedId.hasTemplateKeyword = hasTemplateKeyword;
		return result;
	}

	static AstNode* GenerateTypeIdNode(AstNode* typeSpecifierSeq, AstNode* abstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypeId);
		result->typeIdNode.typeSpecifierSeq = typeSpecifierSeq;
		result->typeIdNode.abstractDeclarator = abstractDeclarator;
		return result;
	}

	static AstNode* GenerateEnumNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumName);
		result->enumName.identifier = identifier;
		return result;
	}

	static AstNode* GenerateEnumSpecifierNode(AstNode* enumHead, AstNode* enumeratorList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumSpecifier);
		result->enumSpecifier.enumHead = enumHead;
		result->enumSpecifier.enumeratorList = enumeratorList;
		return result;
	}

	static AstNode* GenerateEnumKeyNode(EnumKeyType type)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumKey);
		result->enumKey.type = type;
		return result;
	}

	static AstNode* GenerateEnumHeadNode(AstNode* enumKey, AstNode* attributeSpecifierSeq, AstNode* nestedNameSpecifier, Token identifier, AstNode* enumBase)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumHead);
		result->enumHead.enumKey = enumKey;
		result->enumHead.attributeSpecifierSeq = attributeSpecifierSeq;
		result->enumHead.nestedNameSpecifier = nestedNameSpecifier;
		result->enumHead.identifier = identifier;
		result->enumHead.enumBase = enumBase;
		return result;
	}

	static AstNode* GenerateOpaqueEnumDeclNode(AstNode* enumKey, AstNode* attributeSpecifierSeq, Token identifier, AstNode* enumBase)
	{
		AstNode* result = GenerateAstNode(AstNodeType::OpaqueEnumDecl);
		result->opaqueEnumDecl.enumKey = enumKey;
		result->opaqueEnumDecl.attributeSpecifierSeq = attributeSpecifierSeq;
		result->opaqueEnumDecl.identifier = identifier;
		result->opaqueEnumDecl.enumBase = enumBase;
		return result;
	}

	static AstNode* GenerateEnumBaseNode(AstNode* typeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumBase);
		result->enumBase.TypeSpecifierSeq = typeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateEnumeratorListNode(AstNode* enumDefinition, AstNode* nextEnumDefinition)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumeratorList);
		result->enumeratorList.enumDefinition = enumDefinition;
		result->enumeratorList.nextEnumDefinition = nextEnumDefinition;
		return result;
	}

	static AstNode* GenerateEnumeratorDefinitionNode(Token identifier, AstNode* value)
	{
		AstNode* result = GenerateAstNode(AstNodeType::EnumeratorDefinition);
		result->enumeratorDefinition.identifier = identifier;
		result->enumeratorDefinition.value = value;
		return result;
	}

	static AstNode* GenerateConstantExpressionNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ConstantExpression);
		result->constantExpression.expression = expression;
		return result;
	}

	static AstNode* GenerateIfElseNode(AstNode* condition, AstNode* ifStatement, AstNode* elseStatement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IfElse);
		result->ifElseNode.condition = condition;
		result->ifElseNode.ifStatement = ifStatement;
		result->ifElseNode.elseStatement = elseStatement;
		return result;
	}

	static AstNode* GenerateSwitchNode(AstNode* condition, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Switch);
		result->switchNode.condition = condition;
		result->switchNode.statement = statement;
		return result;
	}

	static AstNode* GenerateInitializerConditionNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator, AstNode* initializerClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::InitializerCondition);
		result->initCondition.attributeSpecifierSeq = attributeSpecifierSeq;
		result->initCondition.declSpecifierSeq = declSpecifierSeq;
		result->initCondition.declarator = declarator;
		result->initCondition.initializerClause = initializerClause;
		return result;
	}

	static AstNode* GenerateBracedInitConditionNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BracedInitCondition);
		result->bracedInitCondition.attributeSpecifierSeq = attributeSpecifierSeq;
		result->bracedInitCondition.declSpecifierSeq = declSpecifierSeq;
		result->bracedInitCondition.declarator = declarator;
		result->bracedInitCondition.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GenerateWhileLoopNode(AstNode* condition, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::WhileLoop);
		result->whileLoopNode.condition = condition;
		result->whileLoopNode.statement = statement;
		return result;
	}

	static AstNode* GenerateDoWhileLoopNode(AstNode* statement, AstNode* condition)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DoWhileLoop);
		result->whileLoopNode.statement = statement;
		result->whileLoopNode.condition = condition;
		return result;
	}

	static AstNode* GenerateForLoopNode(AstNode* forInitStatement, AstNode* condition, AstNode* expression, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ForLoop);
		result->forLoopNode.forInitStatement = forInitStatement;
		result->forLoopNode.condition = condition;
		result->forLoopNode.expression = expression;
		result->forLoopNode.statement = statement;
		return result;
	}

	static AstNode* GenerateForEachLoopNode(AstNode* forRangeDeclaration, AstNode* forRangeInitializer, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ForEachLoop);
		result->forEachLoopNode.forRangeDeclaration = forRangeDeclaration;
		result->forEachLoopNode.forRangeInitializer = forRangeInitializer;
		result->forEachLoopNode.statement = statement;
		return result;
	}

	static AstNode* GenerateForRangeDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* typeSpecifierSeq, AstNode* declarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ForRangeDeclaration);
		result->forRangeDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->forRangeDeclaration.typeSpecifierSeq = typeSpecifierSeq;
		result->forRangeDeclaration.declarator = declarator;
		return result;
	}

	static AstNode* GenerateForRangeInitializerNode(AstNode* expression, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ForRangeInitializer);
		result->forRangeInitializer.expression = expression;
		result->forRangeInitializer.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GenerateStatementNode(AstNode* attributeSpecifierSeq, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Statement);
		result->statement.attributeSpecifierSeq = attributeSpecifierSeq;
		result->statement.statement = statement;
		return result;
	}

	static AstNode* GenerateQualifiedIdNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::QualifiedId);
		result->qualifeidId.identifier = identifier;
		return result;
	}

	static AstNode* GenerateLabeledIdentifierNode(AstNode* attributeSpecifierSeq, Token identifier, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LabeledIdentifier);
		result->labeledIdentifier.attributeSpecifierSeq = attributeSpecifierSeq;
		result->labeledIdentifier.identifier = identifier;
		result->labeledIdentifier.statement = statement;
		return result;
	}

	static AstNode* GenerateCaseLabelNode(AstNode* attributeSpecifierSeq, AstNode* constantExpression, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CaseLabel);
		result->caseLabel.attributeSpecifierSeq = attributeSpecifierSeq;
		result->caseLabel.constantExpression = constantExpression;
		result->caseLabel.statement = statement;
		return result;
	}

	static AstNode* GenerateDefaultLabelNode(AstNode* attributeSpecifierSeq, AstNode* statement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DefaultLabel);
		result->defaultLabel.attributeSpecifierSeq = attributeSpecifierSeq;
		result->defaultLabel.statement = statement;
		return result;
	}

	static AstNode* GenerateCompoundStatementNode(AstNode* statementSequence)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CompoundStatement);
		result->compoundStatement.statementSequence = statementSequence;
		return result;
	}

	static AstNode* GenerateStatementSequenceNode(AstNode* thisStatement, AstNode* nextStatement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::StatementSequence);
		result->statementSequence.thisStatement = thisStatement;
		result->statementSequence.nextStatement = nextStatement;
		return result;
	}

	static AstNode* GenerateBreakNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::Break);
		return result;
	}

	static AstNode* GenerateContinueNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::Continue);
		return result;
	}

	static AstNode* GenerateReturnNode(AstNode* returnValue)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Return);
		result->returnNode.returnValue = returnValue;
		return result;
	}

	static AstNode* GenerateGotoNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Goto);
		result->gotoNode.identifier = identifier;
		return result;
	}

	static AstNode* GenerateNamespaceNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NamespaceName);
		result->namespaceNameNode.identifier = identifier;
		return result;
	}

	static AstNode* GenerateNamedNamespaceDefinitionNode(bool isInline, Token identifier, AstNode* namespaceBody)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NamedNamespaceDefinition);
		result->namedNamespaceDefinition.isInline = isInline;
		result->namedNamespaceDefinition.identifier = identifier;
		result->namedNamespaceDefinition.namespaceBody = namespaceBody;
		return result;
	}

	static AstNode* GenerateUnnamedNamespaceDefinitionNode(bool isInline, AstNode* namespaceBody)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UnnamedNamespaceDefinition);
		result->unnamedNamespaceDefinition.isInline = isInline;
		result->unnamedNamespaceDefinition.namespaceBody = namespaceBody;
		return result;
	}

	static AstNode* GenerateNamespaceAliasDefinitionNode(Token identifier, AstNode* qualifiedNamespaceSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NamespaceAliasDefinition);
		result->namespaceAliasDefinition.identifier = identifier;
		result->namespaceAliasDefinition.qualifiedNamespaceSpecifier = qualifiedNamespaceSpecifier;
		return result;
	}

	static AstNode* GenerateQualifiedNamespaceSpecifierNode(bool isNested, AstNode* nestedNameSpecifier, AstNode* namespaceName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::QualifiedNamespaceSpecifier);
		result->qualifiedNamespaceSpecifier.isNested = isNested;
		result->qualifiedNamespaceSpecifier.nestedNameSpecifier = nestedNameSpecifier;
		result->qualifiedNamespaceSpecifier.namespaceName = namespaceName;
		return result;
	}

	static AstNode* GenerateUsingDeclarationNode(AstNode* unqualifiedId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UsingDeclaration);
		result->usingDeclaration.unqualifiedId = unqualifiedId;
		return result;
	}

	static AstNode* GenerateUsingTypenameDeclarationNode(bool hasTypename, bool isNested, AstNode* nestedNamespaceSpecifier, AstNode* unqualifiedId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UsingTypenameDeclaration);
		result->usingTypenameDeclaration.hasTypename = hasTypename;
		result->usingTypenameDeclaration.isNested = isNested;
		result->usingTypenameDeclaration.nestedNameSpecifier = nestedNamespaceSpecifier;
		result->usingTypenameDeclaration.unqualifiedId = unqualifiedId;
		return result;
	}

	static AstNode* GenerateUsingDirectiveNode(AstNode* attributeSpecifierSeq, bool isNested, AstNode* nestedNameSpecifier, AstNode* namespaceName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UsingDirective);
		result->usingDirective.attributeSpecifierSeq = attributeSpecifierSeq;
		result->usingDirective.isNested = isNested;
		result->usingDirective.nestedNameSpecifier = nestedNameSpecifier;
		result->usingDirective.namespaceName = namespaceName;
		return result;
	}

	static AstNode* GenerateAsmNode(Token stringLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Asm);
		result->asmNode.stringLiteral = stringLiteral;
		return result;
	}

	static AstNode* GenerateLinkageSpecificationBlockNode(Token stringLiteral, AstNode* declarationSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LinkageSpecificationBlock);
		result->linkageSpecificationBlock.stringLiteral = stringLiteral;
		result->linkageSpecificationBlock.declarationSeq = declarationSeq;
		return result;
	}

	static AstNode* GenerateLinkageSpecificationNode(Token stringLiteral, AstNode* declaration)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LinkageSpecification);
		result->linkageSpecification.stringLiteral = stringLiteral;
		result->linkageSpecification.declaration = declaration;
		return result;
	}

	static AstNode* GenerateAttributeSpecifierSequenceNode(AstNode* thisSpec, AstNode* nextSpec)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AttributeSpecifierSequence);
		result->attributeSpecifierSeq.thisSpec = thisSpec;
		result->attributeSpecifierSeq.nextSpec = nextSpec;
		return result;
	}

	static AstNode* GenerateAlignAsExpressionNode(AstNode* alignmentExpression, bool hasElipsis)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AlignAsExpression);
		result->alignAsExpression.alignmentExpression = alignmentExpression;
		result->alignAsExpression.hasElipsis = hasElipsis;
		return result;
	}

	static AstNode* GenerateAlignAsTypeIdNode(AstNode* typeId, bool hasElipsis)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AlignAsTypeId);
		result->alignAsTypeId.typeId = typeId;
		result->alignAsTypeId.hasElipsis = hasElipsis;
		return result;
	}

	static AstNode* GenerateAttributeListNode(AstNode* thisAttribute, AstNode* nextAttribute)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AttributeList);
		result->attributeList.thisAttribute = thisAttribute;
		result->attributeList.nextAttribute = nextAttribute;
		return result;
	}

	static AstNode* GenerateEmptyAttributeListNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::EmptyAttributeList);
		return result;
	}

	static AstNode* GenerateAttributeNode(AstNode* attributeToken, AstNode* attributeArgumentClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Attribute);
		result->attribute.attributeToken = attributeToken;
		result->attribute.attributeArgumentClause = attributeArgumentClause;
		return result;
	}

	static AstNode* GenerateAttributeTokenNode(Token namespaceName, Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AttributeToken);
		result->attributeToken.namespaceName = namespaceName;
		result->attributeToken.identifier = identifier;
		return result;
	}

	static AstNode* GenerateAttributeArgumentClauseNode(AstNode* balancedTokenSequence)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AttributeArgumentClause);
		result->attributeArgumentClause.balancedTokenSequence = balancedTokenSequence;
		return result;
	}

	static AstNode* GenerateBalancedTokenSeqNode(AstNode* thisToken, AstNode* nextToken)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BalancedTokenSeq);
		result->balancedTokenSeq.thisToken = thisToken;
		result->balancedTokenSeq.nextToken = nextToken;
		return result;
	}

	static AstNode* GenerateBalancedTokenNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BalancedToken);
		result->balancedToken.token = token;
		return result;
	}

	static AstNode* GenerateInitDeclaratorListNode(AstNode* thisDeclarator, AstNode* nextDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::InitDeclaratorList);
		result->initDeclaratorList.thisDeclarator = thisDeclarator;
		result->initDeclaratorList.nextDeclarator = nextDeclarator;
		return result;
	}

	static AstNode* GenerateInitDeclaratorNode(AstNode* declarator, AstNode* initializer)
	{
		AstNode* result = GenerateAstNode(AstNodeType::InitDeclarator);
		result->initDeclarator.declarator = declarator;
		result->initDeclarator.initializer = initializer;
		return result;
	}

	static AstNode* GenerateDeclaratorNode(AstNode* noPtrDeclarator, AstNode* parametersAndQualifiers, AstNode* trailingReturnType)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Declarator);
		result->declaratorNode.noPtrDeclarator = noPtrDeclarator;
		result->declaratorNode.parametersAndQualifiers = parametersAndQualifiers;
		result->declaratorNode.trailingReturnType = trailingReturnType;
		return result;
	}

	static AstNode* GeneratePtrDeclaratorNode(AstNode* ptrOperator, AstNode* ptrDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PtrDeclarator);
		result->ptrDeclarator.ptrOperator = ptrOperator;
		result->ptrDeclarator.ptrDeclarator = ptrDeclarator;
		return result;
	}

	static AstNode* GenerateParametersAndQualifiersNode(AstNode* parameterDeclarationClause,
		AstNode* attributeSpecifierSeq, AstNode* cvQualifierSeq, AstNode* refQualifier, AstNode* exceptionSpecification)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParametersAndQualifiers);
		result->parametersAndQualifiersNode.parameterDeclarationClause = parameterDeclarationClause;
		result->parametersAndQualifiersNode.attributeSpecifierSeq = attributeSpecifierSeq;
		result->parametersAndQualifiersNode.cvQualifierSeq = cvQualifierSeq;
		result->parametersAndQualifiersNode.refQualifier = refQualifier;
		result->parametersAndQualifiersNode.exceptionSpecification = exceptionSpecification;
		return result;
	}

	static AstNode* GenerateTrailingReturnTypeNode(AstNode* trailingTypeSpecifierSeq, AstNode* abstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TrailingReturnType);
		result->trailingReturnTypeNode.trailingTypeSpecifierSeq = trailingTypeSpecifierSeq;
		result->trailingReturnTypeNode.abstractDeclarator = abstractDeclarator;
		return result;
	}

	static AstNode* GeneratePtrNamespaceStarNode(AstNode* nestedNameSpecifier, AstNode* attributeSpecifierSeq, AstNode* cvQualifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PtrNamespaceStar);
		result->ptrNamespaceStar.nestedNameSpecifier = nestedNameSpecifier;
		result->ptrNamespaceStar.attributeSpecifierSeq = attributeSpecifierSeq;
		result->ptrNamespaceStar.cvQualifierSeq = cvQualifierSeq;
		return result;
	}

	static AstNode* GenerateRefRefNode(AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::RefRef);
		result->refRef.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateRefNode(AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Ref);
		result->ref.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GeneratePtrStarNode(AstNode* attributeSpecifierSeq, AstNode* cvQualifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PtrStar);
		result->ptrStar.attributeSpecifierSeq = attributeSpecifierSeq;
		result->ptrStar.cvQualifierSeq = cvQualifierSeq;
		return result;
	}

	static AstNode* GenerateCvQualifierSeqNode(AstNode* thisQualifier, AstNode* nextQualifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CvQualifierSeq);
		result->cvQualifierSeq.thisQualifier = thisQualifier;
		result->cvQualifierSeq.nextQualifier = nextQualifier;
		return result;
	}

	static AstNode* GenerateCvQualifierNode(Token qualifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CvQualifier);
		result->cvQualifier.qualifier = qualifier;
		return result;
	}

	static AstNode* GenerateRefQualifierNode(bool doubleRef)
	{
		AstNode* result = GenerateAstNode(AstNodeType::RefQualifier);
		result->refQualifier.doubleRef = doubleRef;
		return result;
	}

	static AstNode* GenerateDeclaratorIdNode(AstNode* nestedNameSpecifier, AstNode* className)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DeclaratorId);
		result->declaratorId.nestedNameSpecifier = nestedNameSpecifier;
		result->declaratorId.className = className;
		return result;
	}

	static AstNode* GenerateClassNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassName);
		result->className.identifier = identifier;
		return result;
	}

	static AstNode* GenerateClassSpecifierNode(AstNode* classHead, AstNode* memberSpecification)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassSpecifier);
		result->classSpecifier.classHead = classHead;
		result->classSpecifier.memberSpecification = memberSpecification;
		return result;
	}

	static AstNode* GenerateClassVirtualHeadNode(AstNode* classKey, AstNode* attributeSpecifierSeq, AstNode* classHeadName, AstNode* classVirtSpecifierSeq, AstNode* baseClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassVirtualHead);
		result->classVirtualHead.classKey = classKey;
		result->classVirtualHead.attributeSpecifierSeq = attributeSpecifierSeq;
		result->classVirtualHead.classHeadName = classHeadName;
		result->classVirtualHead.classVirtSpecifierSeq = classVirtSpecifierSeq;
		result->classVirtualHead.baseClause = baseClause;
		return result;
	}

	static AstNode* GenerateClassHeadNode(AstNode* classKey, AstNode* attributeSpecifierSeq, AstNode* baseClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassHead);
		result->classHead.classKey = classKey;
		result->classHead.attributeSpecifierSeq = attributeSpecifierSeq;
		result->classHead.baseClause = baseClause;
		return result;
	}

	static AstNode* GenerateClassHeadNameNode(AstNode* nestedNameSpecifier, AstNode* className)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassHeadName);
		result->classHeadName.nestedNameSpecifier = nestedNameSpecifier;
		result->classHeadName.className = className;
		return result;
	}

	static AstNode* GenerateClassVirtSpecifierSeqNode(AstNode* thisSpec, AstNode* nextSpec)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassVirtSpecifierSeq);
		result->classVirtSpecifierSeq.thisSpec = thisSpec;
		result->classVirtSpecifierSeq.nextSpec = nextSpec;
		return result;
	}

	static AstNode* GenerateClassVirtSpecifierNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassVirtSpecifier);
		result->classVirtSpecifier.token = token;
		return result;
	}

	static AstNode* GenerateClassKeyNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassKey);
		result->classKey.token = token;
		return result;
	}

	static AstNode* GenerateMemberAndAccessSpecifierNode(AstNode* accessSpecifier, AstNode* memberSpecification)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberAndAccessSpecifier);
		result->memberAndAccessSpecifier.accessSpecifier = accessSpecifier;
		result->memberAndAccessSpecifier.memberSpecification = memberSpecification;
		return result;
	}

	static AstNode* GenerateMemberSpecifierNode(AstNode* memberDeclaration, AstNode* memberSpecification)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberSpecifier);
		result->memberSpecifier.memberDeclaration = memberDeclaration;
		result->memberSpecifier.memberSpecification = memberSpecification;
		return result;
	}

	static AstNode* GenerateMemberFunctionDeclarationNode(AstNode* functionDefinition, bool hasTrailingSemicolon)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberFunctionDeclaration);
		result->memberFunctionDeclaration.functionDefinition = functionDefinition;
		result->memberFunctionDeclaration.hasTrailingSemicolon = hasTrailingSemicolon;
		return result;
	}

	static AstNode* GenerateMemberDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* memberDeclaratorList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberDeclaration);
		result->memberDeclarationNode.attribtueSpecifierSeq = attributeSpecifierSeq;
		result->memberDeclarationNode.declSpecifierSeq = declSpecifierSeq;
		result->memberDeclarationNode.memberDeclaratorList = memberDeclaratorList;
		return result;
	}

	static AstNode* GenerateMemberDeclaratorListNode(AstNode* thisDeclarator, AstNode* nextDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberDeclaratorList);
		result->memberDeclaratorList.thisDeclarator = thisDeclarator;
		result->memberDeclaratorList.nextDeclarator = nextDeclarator;
		return result;
	}

	static AstNode* GenerateMemberDeclaratorPureNode(AstNode* declarator, AstNode* virtSpecifierSeq, AstNode* pureSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberDeclaratorPure);
		result->memberDeclaratorPure.declarator = declarator;
		result->memberDeclaratorPure.virtSpecifierSeq = virtSpecifierSeq;
		result->memberDeclaratorPure.pureSpecifier = pureSpecifier;
		return result;
	}

	static AstNode* GenerateMemberDeclaratorBraceNode(AstNode* declarator, AstNode* virtSpecifierSeq, AstNode* braceOrEqualInitializer)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberDeclaratorBrace);
		result->memberDeclaratorBrace.declarator = declarator;
		result->memberDeclaratorBrace.virtSpecifierSeq = virtSpecifierSeq;
		result->memberDeclaratorBrace.braceOrEqualInitializer = braceOrEqualInitializer;
		return result;
	}

	static AstNode* GenerateMemberDeclaratorNode(Token identifier, AstNode* attributeSpecifierSeq, AstNode* virtSpecifierSeq, AstNode* constantExpression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemberDeclarator);
		result->memberDeclarator.identifier = identifier;
		result->memberDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		result->memberDeclarator.virtSpecifierSeq = virtSpecifierSeq;
		result->memberDeclarator.constantExpression = constantExpression;
		return result;
	}

	static AstNode* GenerateVirtSpecifierSeqNode(AstNode* thisSpec, AstNode* nextSpec)
	{
		AstNode* result = GenerateAstNode(AstNodeType::VirtSpecifierSeq);
		result->virtSpecifierSeq.thisSpec = thisSpec;
		result->virtSpecifierSeq.nextSpec = nextSpec;
		return result;
	}

	static AstNode* GenerateVirtSpecifierNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::VirtSpecifier);
		result->virtSpecifier.token = token;
		return result;
	}

	static AstNode* GeneratePureSpecifierNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::PureSpecifier);
		return result;
	}

	static AstNode* GenerateBaseSpecifierListNode(AstNode* thisBaseSpecifier, AstNode* nextBaseSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BaseSpecifierList);
		result->baseSpecifierList.thisBaseSpecifier = thisBaseSpecifier;
		result->baseSpecifierList.nextBaseSpecifier = nextBaseSpecifier;
		return result;
	}

	static AstNode* GenerateBaseSpecifierNode(AstNode* attributeSpecifierSeq, bool isVirtual, AstNode* accessSpecifier, AstNode* baseTypeSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::BaseSpecifier);
		result->baseSpecifier.attributeSpecifierSeq = attributeSpecifierSeq;
		result->baseSpecifier.isVirtual = isVirtual;
		result->baseSpecifier.accessSpecifier = accessSpecifier;
		result->baseSpecifier.baseTypeSpecifier = baseTypeSpecifier;
		return result;
	}

	static AstNode* GenerateClassOrDecltypeNode(AstNode* nestedNameSpecifier, AstNode* className)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ClassOrDecltype);
		result->classOrDecltype.nestedNameSpecifier = nestedNameSpecifier;
		result->classOrDecltype.className = className;
		return result;
	}

	static AstNode* GenerateAccessSpecifierNode(Token accessSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AccessSpecifier);
		result->accessSpecifier.accessSpecifier = accessSpecifier;
		return result;
	}

	static AstNode* GenerateConversionFunctionIdNode(AstNode* conversionTypeId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ConversionFunctionId);
		result->conversionFunctionId.conversionTypeId = conversionTypeId;
		return result;
	}

	static AstNode* GenerateConversionTypeIdNode(AstNode* typeSpecifierSeq, AstNode* conversionDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ConversionTypeId);
		result->conversionTypeId.typeSpecifierSeq = typeSpecifierSeq;
		result->conversionTypeId.conversionDeclarator = conversionDeclarator;
		return result;
	}

	static AstNode* GenerateConversionDeclaratorNode(AstNode* ptrOperator, AstNode* conversionDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ConversionDeclarator);
		result->conversionDeclarator.ptrOperator = ptrOperator;
		result->conversionDeclarator.conversionDeclarator = conversionDeclarator;
		return result;
	}

	static AstNode* GenerateMemInitializerListNode(AstNode* thisMemInitializer, AstNode* nextMemInitializer)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemInitializerList);
		result->memInitializerList.thisMemInitializer = thisMemInitializer;
		result->memInitializerList.nextMemInitializer = nextMemInitializer;
		return result;
	}

	static AstNode* GenerateMemExpressionInitializerNode(AstNode* memInitializerId, AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemExpressionInitializer);
		result->memExpressionInitializer.memInitializerId = memInitializerId;
		result->memExpressionInitializer.expressionList = expressionList;
		return result;
	}

	static AstNode* GenerateMemBracedInitializerNode(AstNode* memInitializerId, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemBracedInitializer);
		result->memBracedInitializer.memInitializerId = memInitializerId;
		result->memBracedInitializer.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GenerateMemInitializerIdNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MemInitializerId);
		result->memInitializerId.identifier = identifier;
		return result;
	}

	static AstNode* GenerateNestedNamespaceSpecifierIdNode(AstNode* nestedNameSpecifier, Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NestedNamespaceSpecifierId);
		result->nestedNamespaceSpecifierId.nestedNameSpecifier = nestedNameSpecifier;
		result->nestedNamespaceSpecifierId.identifier = identifier;
		return result;
	}

	static AstNode* GenerateNestedNamespaceSpecifierTemplateNode(AstNode* nestedNameSpecifier, bool hasTemplateKeyword, AstNode* simpleTemplateId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NestedNamespaceSpecifierTemplate);
		result->nestedNamespaceSpecifierTemplate.nestedNameSpecifier = nestedNameSpecifier;
		result->nestedNamespaceSpecifierTemplate.hasTemplateKeyword = hasTemplateKeyword;
		result->nestedNamespaceSpecifierTemplate.simpleTemplateId = simpleTemplateId;
		return result;
	}

	static AstNode* GeneratePostfixSimpleTypeExpressionListNode(AstNode* simpleTypeSpecifier, AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixSimpleTypeExpressionList);
		result->postfixSimpleTypeExpressionList.simpleTypeSpecifier = simpleTypeSpecifier;
		result->postfixSimpleTypeExpressionList.expressionList = expressionList;
		return result;
	}

	static AstNode* GeneratePostfixSimpleTypeBraceListNode(AstNode* simpleTypeSpecifier, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixSimpleTypeBraceList);
		result->postfixSimpleTypeBraceList.simpleTypeSpecifier = simpleTypeSpecifier;
		result->postfixSimpleTypeBraceList.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GeneratePostfixTypenameSpecExpressionListNode(AstNode* typenameSpecifier, AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixTypenameSpecExpressionList);
		result->postfixTypenameSpecExpressionList.typenameSpecifier = typenameSpecifier;
		result->postfixTypenameSpecExpressionList.expressionList = expressionList;
		return result;
	}

	static AstNode* GeneratePostfixTypenameSpecBraceListNode(AstNode* typenameSpecifier, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixTypenameSpecBraceList);
		result->postfixTypenameSpecBraceList.typenameSpecifier = typenameSpecifier;
		result->postfixTypenameSpecBraceList.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GeneratePostfixCastNode(AstNode* typeId, AstNode* expression, CastType castType)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixCast);
		result->postfixCast.typeId = typeId;
		result->postfixCast.expression = expression;
		result->postfixCast.castType = castType;
		return result;
	}

	static AstNode* GeneratePostfixTypeIdExpressionNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixTypeIdExpression);
		result->postfixTypeIdExpression.expression = expression;
		return result;
	}

	static AstNode* GeneratePostfixTypeIdNode(AstNode* typeId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixTypeId);
		result->postfixTypeId.typeId = typeId;
		return result;
	}

	static AstNode* GeneratePostfixBracketExpressionNode(AstNode* postfixExpression, AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixBracketExpression);
		result->postfixBracketExpression.postfixExpression = postfixExpression;
		result->postfixBracketExpression.expression = expression;
		return result;
	}

	static AstNode* GeneratePostfixBracketBraceListNode(AstNode* postfixExpression, AstNode* bracedInitList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixBracketBraceList);
		result->postfixBracketBraceList.postfixExpression = postfixExpression;
		result->postfixBracketBraceList.bracedInitList = bracedInitList;
		return result;
	}

	static AstNode* GeneratePostfixParenExpressionListNode(AstNode* postfixExpression, AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixParenExpressionList);
		result->postfixParenExpressionList.postfixExpression = postfixExpression;
		result->postfixParenExpressionList.expressionList = expressionList;
		return result;
	}

	static AstNode* GeneratePostfixMemberIdExpressionNode(AstNode* postfixExpression, AstNode* idExpression, bool hasTemplateKeyword, MemberOperatorType memberOp)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixMemberIdExpression);
		result->postfixMemberIdExpression.postfixExpression = postfixExpression;
		result->postfixMemberIdExpression.idExpression = idExpression;
		result->postfixMemberIdExpression.hasTemplateKeyword = hasTemplateKeyword;
		result->postfixMemberIdExpression.memberOp = memberOp;
		return result;
	}

	static AstNode* GeneratePostfixPseudoDestructorNode(AstNode* postfixExpression, AstNode* pseudoDestructorName, MemberOperatorType memberOp)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixPseudoDestructor);
		result->postfixPseudoDestructor.postfixExpression = postfixExpression;
		result->postfixPseudoDestructor.pseudoDestructorName = pseudoDestructorName;
		result->postfixPseudoDestructor.memberOp = memberOp;
		return result;
	}

	static AstNode* GeneratePostfixPlusPlusNode(AstNode* postfixExpression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixPlusPlus);
		result->postfixPlusPlus.postfixExpression = postfixExpression;
		return result;
	}

	static AstNode* GeneratePostfixMinusMinusNode(AstNode* postfixExpression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PostfixMinusMinus);
		result->postfixMinusMinus.postfixExpression = postfixExpression;
		return result;
	}

	static AstNode* GeneratePseudoDestructorDecltypeNode(AstNode* decltypeSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PseudoDestructorDecltype);
		result->pseudoDestructorDecltype.decltypeSpecifier = decltypeSpecifier;
		return result;
	}

	static AstNode* GeneratePseudoDestructorTemplateNode(AstNode* nestedNameSpecifier, AstNode* simpleTemplateId, AstNode* typeName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PseudoDestructorTemplate);
		result->pseudoDestructorTemplate.nestedNameSpecifier = nestedNameSpecifier;
		result->pseudoDestructorTemplate.simpleTemplateId = simpleTemplateId;
		result->pseudoDestructorTemplate.typeName = typeName;
		return result;
	}

	static AstNode* GeneratePseudoDestructorNode(AstNode* nestedNameSpecifier, AstNode* typeName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PseudoDestructor);
		result->pseudoDestructor.nestedNameSpecifier = nestedNameSpecifier;
		result->pseudoDestructor.typeName = typeName;
		return result;
	}

	static AstNode* GeneratePseudoNestedDestructorNode(AstNode* nestedNameSpecifier, AstNode* nestedTypeName, AstNode* typeName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PseudoNestedDestructor);
		result->pseudoNestedDestructor.nestedNameSpecifier = nestedNameSpecifier;
		result->pseudoNestedDestructor.nestedTypeName = nestedTypeName;
		result->pseudoNestedDestructor.typeName = typeName;
		return result;
	}

	static AstNode* GenerateNewTypeIdExpressionNode(AstNode* newPlacement, AstNode* newTypeId, AstNode* newInitializer)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewTypeIdExpression);
		result->newTypeIdExpression.newPlacement = newPlacement;
		result->newTypeIdExpression.newTypeId = newTypeId;
		result->newTypeIdExpression.newInitializer = newInitializer;
		return result;
	}

	static AstNode* GenerateNewExpressionNode(AstNode* newPlacement, AstNode* typeId, AstNode* newInitializer)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewExpression);
		result->newExpression.newPlacement = newPlacement;
		result->newExpression.typeId = typeId;
		result->newExpression.newInitializer = newInitializer;
		return result;
	}

	static AstNode* GenerateNewPlacementNode(AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewPlacement);
		result->newPlacementNode.expressionList = expressionList;
		return result;
	}

	static AstNode* GenerateNewTypeIdNode(AstNode* typeSpecifierSeq, AstNode* newDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewTypeId);
		result->newTypeId.typeSpecifierSeq = typeSpecifierSeq;
		result->newTypeId.newDeclarator = newDeclarator;
		return result;
	}

	static AstNode* GenerateNewDeclaratorNode(AstNode* ptrOperator, AstNode* newDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewDeclarator);
		result->newDeclarator.ptrOperator = ptrOperator;
		result->newDeclarator.newDeclarator = newDeclarator;
		return result;
	}

	static AstNode* GenerateNoptrNewTailDeclaratorNode(AstNode* expression, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrNewTailDeclarator);
		result->noptrNewTailDeclarator.expression = expression;
		result->noptrNewTailDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateNoptrNewDeclaratorNode(AstNode* noptrNewDeclarator, AstNode* constantExpression, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrNewDeclarator);
		result->noptrNewDeclarator.noptrNewDeclarator = noptrNewDeclarator;
		result->noptrNewDeclarator.constantExpression = constantExpression;
		result->noptrNewDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateNewInitializerNode(AstNode* expressionList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NewInitializer);
		result->newInitializer.expressionList = expressionList;
		return result;
	}

	static AstNode* GenerateDeclarationSeqNode(AstNode* thisDeclaration, AstNode* nextDeclaration)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DeclarationSeq);
		result->declarationSeq.thisDeclaration = thisDeclaration;
		result->declarationSeq.nextDeclaration = nextDeclaration;
		return result;
	}

	static AstNode* GenerateAliasDeclarationNode(Token identifier, AstNode* typeId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AliasDeclaration);
		result->aliasDeclaration.identifier = identifier;
		result->aliasDeclaration.typeId = typeId;
		return result;
	}

	static AstNode* GenerateSimpleDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* initDeclaratorList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleDeclaration);
		result->simpleDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->simpleDeclaration.declSpecifierSeq = declSpecifierSeq;
		result->simpleDeclaration.initDeclaratorList = initDeclaratorList;
		return result;
	}

	static AstNode* GenerateStaticAssertDeclarationNode(AstNode* constantExpression, Token stringLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::StaticAssertDeclaration);
		result->staticAssertDeclaration.constantExpression = constantExpression;
		result->staticAssertDeclaration.stringLiteral = stringLiteral;
		return result;
	}

	static AstNode* GenerateSimpleDeclSpecifierNode(Token token)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleDeclSpecifier);
		result->simpleDeclSpecifier.token = token;
		return result;
	}

	static AstNode* GenerateDeclSpecifierNode(AstNode* specifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DeclSpecifier);
		result->declSpecifier.specifier = specifier;
		return result;
	}

	static AstNode* GenerateDeclSpecSeqNode(AstNode* thisSpec, AstNode* nextSpec, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DeclSpecSeq);
		result->declSpecSeq.thisSpec = thisSpec;
		result->declSpecSeq.nextSpec = nextSpec;
		result->declSpecSeq.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateStorageClassSpecNode(Token specifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::StorageClassSpec);
		result->storageClassSpec.specifier = specifier;
		return result;
	}

	static AstNode* GenerateFunctionSpecNode(Token specifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionSpec);
		result->functionSpec.specifier = specifier;
		return result;
	}

	static AstNode* GenerateTypedefNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypedefName);
		result->typedefName.identifier = identifier;
		return result;
	}

	static AstNode* GenerateTypeSpecSeqNode(AstNode* thisTypeSpec, AstNode* nextTypeSpec, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypeSpecSeq);
		result->typeSpecSeq.thisTypeSpec = thisTypeSpec;
		result->typeSpecSeq.nextTypeSpec = nextTypeSpec;
		result->typeSpecSeq.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateTrailingTypeSpecSeqNode(AstNode* thisTypeSpec, AstNode* nextTypeSpec, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TrailingTypeSpecSeq);
		result->trailingTypeSpecSeq.thisTypeSpec = thisTypeSpec;
		result->trailingTypeSpecSeq.nextTypeSpec = nextTypeSpec;
		result->trailingTypeSpecSeq.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateSimpleTypeTokenSpecNode(Token type)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleTypeTokenSpec);
		result->simpleTypeTokenSpec.type = type;
		return result;
	}

	static AstNode* GenerateSimpleTypeTemplateSpecNode(AstNode* nestedNameSpecifier, AstNode* simpleTemplateId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleTypeTemplateSpec);
		result->simpleTypeTemplateSpec.nestedNameSpec = nestedNameSpecifier;
		result->simpleTypeTemplateSpec.simpleTemplateId = simpleTemplateId;
		return result;
	}

	static AstNode* GenerateSimpleTypeSpecNode(AstNode* nestedNameSpecifier, AstNode* typeName)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleTypeSpec);
		result->simpleTypeSpec.nestedNameSpec = nestedNameSpecifier;
		result->simpleTypeSpec.typeName = typeName;
		return result;
	}

	static AstNode* GenerateDecltypeSpecNode(AstNode* expression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DecltypeSpec);
		result->decltypeSpec.expression = expression;
		return result;
	}

	static AstNode* GenerateAbstractDeclaratorNode(AstNode* noptrAbstractDeclarator, AstNode* parametersAndQualifiers, AstNode* trailingReturnType)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AbstractDeclarator);
		result->abstractDeclarator.noptrAbstractDeclarator = noptrAbstractDeclarator;
		result->abstractDeclarator.parametersAndQualifiers = parametersAndQualifiers;
		result->abstractDeclarator.trailingReturnType = trailingReturnType;
		return result;
	}

	static AstNode* GenerateAbstractElipsisDeclaratorNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::AbstractElipsisDeclarator);
		return result;
	}

	static AstNode* GeneratePtrAbstractDeclaratorNode(AstNode* ptrOperator, AstNode* ptrAbstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PtrAbstractDeclarator);
		result->ptrAbstractDeclarator.ptrOperator = ptrOperator;
		result->ptrAbstractDeclarator.ptrAbstractDeclarator = ptrAbstractDeclarator;
		return result;
	}

	static AstNode* GenerateParameterDeclarationListNode(AstNode* thisParameter, AstNode* nextParameter)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParameterDeclarationList);
		result->parameterDeclarationList.thisParameter = thisParameter;
		result->parameterDeclarationList.nextParameter = nextParameter;
		return result;
	}

	static AstNode* GenerateParameterDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParameterDeclaration);
		result->parameterDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->parameterDeclaration.declSpecifierSeq = declSpecifierSeq;
		result->parameterDeclaration.declarator = declarator;
		return result;
	}

	static AstNode* GenerateParameterDefaultDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator, AstNode* initializerClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParameterDefaultDeclaration);
		result->parameterDefaultDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->parameterDefaultDeclaration.declSpecifierSeq = declSpecifierSeq;
		result->parameterDefaultDeclaration.declarator = declarator;
		result->parameterDefaultDeclaration.initializerClause = initializerClause;
		return result;
	}

	static AstNode* GenerateParameterAbstractDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* abstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParameterAbstractDeclaration);
		result->parameterAbstractDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->parameterAbstractDeclaration.declSpecifierSeq = declSpecifierSeq;
		result->parameterAbstractDeclaration.abstractDeclarator = abstractDeclarator;
		return result;
	}

	static AstNode* GenerateParameterAbstractDefaultDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* abstractDeclarator, AstNode* initializerClause)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ParameterAbstractDefaultDeclaration);
		result->parameterAbstractDefaultDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->parameterAbstractDefaultDeclaration.declSpecifierSeq = declSpecifierSeq;
		result->parameterAbstractDefaultDeclaration.abstractDeclarator = abstractDeclarator;
		result->parameterAbstractDefaultDeclaration.initializerClause = initializerClause;
		return result;
	}

	static AstNode* GenerateFunctionDefaultDefinitionNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator, AutoFunctionType functionType)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionDefaultDefinition);
		result->functionDefaultDefinition.attributeSpecifierSeq = attributeSpecifierSeq;
		result->functionDefaultDefinition.declSpecifierSeq = declSpecifierSeq;
		result->functionDefaultDefinition.declarator = declarator;
		result->functionDefaultDefinition.functionType = functionType;
		return result;
	}

	static AstNode* GenerateFunctionDefinitionNode(AstNode* attributeSpecifierSeq, AstNode* declSpecifierSeq, AstNode* declarator, AstNode* functionBody)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionDefinition);
		result->functionDefinition.attributeSpecifierSeq = attributeSpecifierSeq;
		result->functionDefinition.declSpecifierSeq = declSpecifierSeq;
		result->functionDefinition.declarator = declarator;
		result->functionDefinition.functionBody = functionBody;
		return result;
	}

	static AstNode* GenerateFunctionBodyNode(AstNode* ctorInitializer, AstNode* compoundStatement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionBody);
		result->functionBody.ctorInitializer = ctorInitializer;
		result->functionBody.compoundStatement = compoundStatement;
		return result;
	}

	static AstNode* GenerateLiteralOperatorIdNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LiteralOperatorId);
		result->literalOperatorId.identifier = identifier;
		return result;
	}

	static AstNode* GenerateTemplateDeclarationNode(AstNode* templateParameterList, AstNode* declaration)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TemplateDeclaration);
		result->templateDeclaration.templateParameterList = templateParameterList;
		result->templateDeclaration.declaration = declaration;
		return result;
	}

	static AstNode* GenerateTemplateParameterListNode(AstNode* thisParameter, AstNode* nextParameter)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TemplateParameterList);
		result->templateParameterList.thisParameter = thisParameter;
		result->templateParameterList.nextParameter = nextParameter;
		return result;
	}

	static AstNode* GenerateSimpleTemplateIdNode(AstNode* templateName, AstNode* templateArgumentList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::SimpleTemplateId);
		result->simpleTemplateId.templateName = templateName;
		result->simpleTemplateId.templateArgumentList = templateArgumentList;
		return result;
	}

	static AstNode* GenerateFunctionOperatorTemplateIdNode(AstNode* operatorFunctionId, AstNode* templateArgumentList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionOperatorTemplateId);
		result->functionOperatorTemplateId.operatorFunctionId = operatorFunctionId;
		result->functionOperatorTemplateId.templateArgumentList = templateArgumentList;
		return result;
	}

	static AstNode* GenerateLiteralOperatorTemplateIdNode(AstNode* literalOperatorId, AstNode* templateArgumentList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::LiteralOperatorTemplateId);
		result->literalOperatorTemplateId.literalOperatorId = literalOperatorId;
		result->literalOperatorTemplateId.templateArgumentList = templateArgumentList;
		return result;
	}

	static AstNode* GenerateTemplateNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TemplateName);
		result->templateName.identifier = identifier;
		return result;
	}

	static AstNode* GenerateTypenameSpecifierNode(AstNode* nestedNameSpecifier, Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypenameSpecifier);
		result->typenameSpecifier.nestedNameSpecifier = nestedNameSpecifier;
		result->typenameSpecifier.identifier = identifier;
		return result;
	}

	static AstNode* GenerateTypenameTemplateSpecifierNode(AstNode* nestedNameSpecifier, AstNode* simpleTemplateId, bool hasTemplateKeyword)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypenameTemplateSpecifier);
		result->typenameTemplateSpecifier.nestedNameSpecifier = nestedNameSpecifier;
		result->typenameTemplateSpecifier.simpleTemplateId = simpleTemplateId;
		result->typenameTemplateSpecifier.hasTemplateKeyword = hasTemplateKeyword;
		return result;
	}

	static AstNode* GenerateExplicitInstantiationNode(AstNode* declaration, bool hasExternKeyword)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ExplicitInstantiation);
		result->explicitInstantiation.declaration = declaration;
		result->explicitInstantiation.hasExternKeyword = hasExternKeyword;
		return result;
	}

	static AstNode* GenerateTryBlockNode(AstNode* compoundStatement, AstNode* handlerSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TryBlock);
		result->tryBlock.compoundStatement = compoundStatement;
		result->tryBlock.handlerSeq = handlerSeq;
		return result;
	}

	static AstNode* GenerateFunctionTryBlockNode(AstNode* ctorInitializer, AstNode* compoundStatement, AstNode* handlerSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::FunctionTryBlock);
		result->functionTryBlock.ctorInitializer = ctorInitializer;
		result->functionTryBlock.compoundStatement = compoundStatement;
		result->functionTryBlock.handlerSeq = handlerSeq;
		return result;
	}

	static AstNode* GenerateHandlerSeqNode(AstNode* thisHandler, AstNode* nextHandler)
	{
		AstNode* result = GenerateAstNode(AstNodeType::HandlerSeq);
		result->handlerSeq.thisHandler = thisHandler;
		result->handlerSeq.nextHandler = nextHandler;
		return result;
	}

	static AstNode* GenerateHandlerNode(AstNode* exceptionDeclaration, AstNode* compoundStatement)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Handler);
		result->handler.exceptionDeclaration = exceptionDeclaration;
		result->handler.compoundStatement = compoundStatement;
		return result;
	}

	static AstNode* GenerateExceptionDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* typeSpecifierSeq, AstNode* declarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ExceptionDeclaration);
		result->exceptionDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->exceptionDeclaration.typeSpecifierSeq = typeSpecifierSeq;
		result->exceptionDeclaration.declarator = declarator;
		return result;
	}

	static AstNode* GenerateExceptionAbstractDeclarationNode(AstNode* attributeSpecifierSeq, AstNode* typeSpecifierSeq, AstNode* abstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ExceptionAbstractDeclaration);
		result->exceptionAbstractDeclaration.attributeSpecifierSeq = attributeSpecifierSeq;
		result->exceptionAbstractDeclaration.typeSpecifierSeq = typeSpecifierSeq;
		result->exceptionAbstractDeclaration.abstractDeclarator = abstractDeclarator;
		return result;
	}

	static AstNode* GenerateThrowExpressionNode(AstNode* assignmentExpression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ThrowExpression);
		result->throwExpression.assignmentExpression = assignmentExpression;
		return result;
	}

	static AstNode* GenerateDynamicExceptionSpecNode(AstNode* typeIdList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::DynamicExceptionSpec);
		result->dynamicExceptionSpec.typeIdList = typeIdList;
		return result;
	}

	static AstNode* GenerateTypeIdListNode(AstNode* thisTypeId, AstNode* nextTypeId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TypeIdList);
		result->typeIdList.thisTypeId = thisTypeId;
		result->typeIdList.nextTypeId = nextTypeId;
		return result;
	}

	static AstNode* GenerateNoexceptExpressionSpecNode(AstNode* constantExpression)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoexceptExpressionSpec);
		result->noexceptExpression.constantExpression = constantExpression;
		return result;
	}

	static AstNode* GenerateNoexceptSpecNode()
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoexceptSpec);
		return result;
	}

	static AstNode* GeneratePreprocessingFileNode(AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PreprocessingFile);
		result->preprocessingFile.group = group;
		return result;
	}

	static AstNode* GenerateGroupNode(AstNode* thisGroupPart, AstNode* nextGroupPart)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Group);
		result->group.thisGroupPart = thisGroupPart;
		result->group.nextGroupPart = nextGroupPart;
		return result;
	}

	static AstNode* GenerateIfSectionNode(AstNode* ifGroup, AstNode* elifGroups, AstNode* elseGroup)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IfSection);
		result->ifSection.ifGroup = ifGroup;
		result->ifSection.elifGroups = elifGroups;
		result->ifSection.elseGroup = elseGroup;
		return result;
	}

	static AstNode* GenerateIfGroupNode(AstNode* constantExpression, AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IfGroup);
		result->ifGroup.constantExpression = constantExpression;
		result->ifGroup.group = group;
		return result;
	}

	static AstNode* GenerateIfDefGroupNode(Token identifier, AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IfDefGroup);
		result->ifDefGroup.identifier = identifier;
		result->ifDefGroup.group = group;
		return result;
	}

	static AstNode* GenerateIfNDefGroupNode(Token identifier, AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IfNDefGroup);
		result->ifNDefGroup.identifier = identifier;
		result->ifNDefGroup.group = group;
		return result;
	}

	static AstNode* GenerateElifGroupsNode(AstNode* thisElifGroup, AstNode* nextElifGroup)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElifGroups);
		result->elifGroups.thisElifGroup = thisElifGroup;
		result->elifGroups.nextElifGroup = nextElifGroup;
		return result;
	}

	static AstNode* GenerateElifGroupNode(AstNode* constantExpression, AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElifGroup);
		result->elifGroup.constantExpression = constantExpression;
		result->elifGroup.group = group;
		return result;
	}

	static AstNode* GenerateElseGroupNode(AstNode* group)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElseGroup);
		result->elseGroup.group = group;
		return result;
	}

	static AstNode* GenerateMacroIncludeNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroInclude);
		result->macroInclude.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateMacroDefineNode(Token identifier, AstNode* replacementList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroDefine);
		result->macroDefine.identifier = identifier;
		result->macroDefine.replacementList = replacementList;
		return result;
	}

	static AstNode* GenerateMacroDefineFunctionNode(Token identifier, AstNode* identifierList, AstNode* replacementList)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroDefineFunction);
		result->macroDefineFunction.identifier = identifier;
		result->macroDefineFunction.identifierList = identifierList;
		result->macroDefineFunction.replacementList = replacementList;
		return result;
	}

	static AstNode* GenerateMacroUndefNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroUndef);
		result->macroUndef.identifier = identifier;
		return result;
	}

	static AstNode* GenerateMacroLineNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroLine);
		result->macroLine.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateMacroErrorNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroError);
		result->macroError.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateMacroPragmaNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::MacroPragma);
		result->macroPragma.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateTextLineNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::TextLine);
		result->textLine.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateNonDirectiveNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NonDirective);
		result->nonDirective.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GenerateIdentifierListNode(AstNode* thisIdentifierNode, AstNode* nextIdentifierNode)
	{
		AstNode* result = GenerateAstNode(AstNodeType::IdentifierList);
		result->identifierList.thisIdentifierNode = thisIdentifierNode;
		result->identifierList.nextIdentifierNode = nextIdentifierNode;
		return result;
	}

	static AstNode* GenerateIdentifierNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::Identifier);
		result->identifier.identifier = identifier;
		return result;
	}

	static AstNode* GenerateReplacementListNode(AstNode* ppTokens)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ReplacementList);
		result->replacementList.ppTokens = ppTokens;
		return result;
	}

	static AstNode* GeneratePPTokensNode(AstNode* preprocessingToken, AstNode* nextPreprocessingToken)
	{
		AstNode* result = GenerateAstNode(AstNodeType::PPTokens);
		result->ppTokens.preprocessingToken = preprocessingToken;
		result->ppTokens.nextPreprocessingToken = nextPreprocessingToken;
		return result;
	}

	static AstNode* GenerateStringLiteralNode(Token stringLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::StringLiteral);
		result->stringLiteral.stringLiteral = stringLiteral;
		return result;
	}

	static AstNode* GenerateNumberLiteralNode(Token numberLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NumberLiteral);
		result->numberLiteral.numberLiteral = numberLiteral;
		return result;
	}

	static AstNode* GenerateCharacterLiteralNode(Token characterLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::CharacterLiteral);
		result->characterLiteral.characterLiteral = characterLiteral;
		return result;
	}

	static AstNode* GenerateHeaderNameNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::HeaderName);
		result->headerName.identifier = identifier;
		return result;
	}

	static AstNode* GenerateHeaderNameStringNode(Token stringLiteral)
	{
		AstNode* result = GenerateAstNode(AstNodeType::HeaderNameString);
		result->headerNameString.stringLiteral = stringLiteral;
		return result;
	}

	static AstNode* GenerateNoptrAbstractDeclaratorNode(AstNode* ptrAbstractDeclarator, AstNode* parametersAndQualifiers, AstNode* noptrAbstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrAbstractDeclarator);
		result->noptrAbstractDeclarator.ptrAbstractDeclarator = ptrAbstractDeclarator;
		result->noptrAbstractDeclarator.parametersAndQualifiers = parametersAndQualifiers;
		result->noptrAbstractDeclarator.noptrAbstractDeclarator = noptrAbstractDeclarator;
		return result;
	}

	static AstNode* GenerateNoptrAbstractExpressionDeclaratorNode(AstNode* ptrAbstractDeclarator, AstNode* constantExpression, AstNode* attributeSpecifierSeq, AstNode* noptrAbstractDeclarator)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrAbstractExpressionDeclarator);
		result->noptrAbstractExpressionDeclarator.ptrAbstractDeclarator = ptrAbstractDeclarator;
		result->noptrAbstractExpressionDeclarator.constantExpression = constantExpression;
		result->noptrAbstractExpressionDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		result->noptrAbstractExpressionDeclarator.noptrAbstractDeclarator = noptrAbstractDeclarator;
		return result;
	}

	static AstNode* GenerateNoptrDeclaratorNode(AstNode* expression, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrDeclarator);
		result->noptrDeclarator.expression = expression;
		result->noptrDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateNoptrConstantDeclaratorNode(AstNode* noptrNewDeclarator, AstNode* constantExpression, AstNode* attributeSpecifierSeq)
	{
		AstNode* result = GenerateAstNode(AstNodeType::NoptrConstantDeclarator);
		result->noptrConstantDeclarator.noptrNewDeclarator = noptrNewDeclarator;
		result->noptrConstantDeclarator.constantExpression = constantExpression;
		result->noptrConstantDeclarator.attributeSpecifierSeq = attributeSpecifierSeq;
		return result;
	}

	static AstNode* GenerateUnqualifiedIdNode(Token identifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UnqualifiedId);
		result->unqualifiedId.identifier = identifier;
		return result;
	}

	static AstNode* GenerateUnqualifiedIdDtorClassNode(AstNode* className)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UnqualifiedIdDtorClass);
		result->unqualifiedIdDtorClass.className = className;
		return result;
	}

	static AstNode* GenerateUnqualifiedIdDtorDecltypeNode(AstNode* decltypeSpecifier)
	{
		AstNode* result = GenerateAstNode(AstNodeType::UnqualifiedIdDtorDecltype);
		result->unqualifiedIdDtorDecltype.decltypeSpecifier = decltypeSpecifier;
		return result;
	}

	static AstNode* GenerateElaboratedSpecifierEnumNode(AstNode* nestedNameSpecifier, Token identifier, bool hasScopeOp)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElaboratedSpecifierEnum);
		result->elaboratedSpecifierEnum.nestedNameSpecifier = nestedNameSpecifier;
		result->elaboratedSpecifierEnum.identifier = identifier;
		result->elaboratedSpecifierEnum.hasScopeOp = hasScopeOp;
		return result;
	}

	static AstNode* GenerateElaboratedSpecifierTemplateNode(AstNode* classKey, AstNode* nestedNameSpecifier, AstNode* simpleTemplateId, bool hasScopeOp, bool hasTemplateKeyword)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElaboratedSpecifierTemplate);
		result->elaboratedSpecifierTemplate.classKey = classKey;
		result->elaboratedSpecifierTemplate.nestedNameSpecifier = nestedNameSpecifier;
		result->elaboratedSpecifierTemplate.simpleTemplateId = simpleTemplateId;
		result->elaboratedSpecifierTemplate.hasScopeOp = hasScopeOp;
		result->elaboratedSpecifierTemplate.hasTemplateKeyword = hasTemplateKeyword;
		return result;
	}

	static AstNode* GenerateElaboratedSpecifierClassNode(AstNode* classKey, AstNode* attributeSpecifierSeq, AstNode* nestedNameSpecifier, Token identifier, bool hasScopeOp)
	{
		AstNode* result = GenerateAstNode(AstNodeType::ElaboratedSpecifierClass);
		result->elaboratedSpecifierClass.classKey = classKey;
		result->elaboratedSpecifierClass.attributeSpecifierSeq = attributeSpecifierSeq;
		result->elaboratedSpecifierClass.nestedNameSpecifier = nestedNameSpecifier;
		result->elaboratedSpecifierClass.identifier = identifier;
		result->elaboratedSpecifierClass.hasScopeOp = hasScopeOp;
		return result;
	}

	static AstNode* GenerateAlignmentExpressionNode(AstNode* typeId)
	{
		AstNode* result = GenerateAstNode(AstNodeType::AlignmentExpression);
		result->alignmentExpression.typeId = typeId;
		return result;
	}

	static AstNode* GenerateNoSuccessAstNode()
	{
		AstNode* node = GenerateAstNode(AstNodeType::None);
		node->success = false;
		return node;
	}

	static TokenType Peek()
	{
		return Tokens[CurrentToken].m_Type;
	}

	static bool PeekIn(std::initializer_list<TokenType> tokenTypes)
	{
		if (std::find(tokenTypes.begin(), tokenTypes.end(), Peek()) != tokenTypes.end())
		{
			return true;
		}
		return false;
	}

	static bool LookAheadBeforeSemicolon(std::initializer_list<TokenType> tokenTypes)
	{
		// This function looks for a token that matches any of the types in the initializer list
		// before the first semicolon or eof token. If it finds it, it returns true, otherwise false
		int token = CurrentToken;
		int tokenTypeSize = tokenTypes.size();
		while (true)
		{
			Token& iter = Tokens[token];
			if (iter.m_Type == TokenType::SEMICOLON)
			{
				return false;
			}

			if (std::find(tokenTypes.begin(), tokenTypes.end(), iter.m_Type) != tokenTypes.end())
			{
				return true;
			}
			token++;
			if (token >= Tokens.size())
			{
				return false;
			}
		}
	}

	static bool IsAlpha(char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
	static bool IsAlphaNumeric(char c)
	{
		return IsAlpha(c) || (c >= '0' && c <= '9');
	}

	static bool IsVariableFriendly(char c)
	{
		return IsAlphaNumeric(c) || c == '_';
	}

	std::string ScriptParser::GetFilenameAsClassName(std::string filename)
	{
		for (int i = 0; i < filename.length(); i++)
		{
			if (i == 0 && !IsAlpha(filename[i]))
			{
				filename[i] = '_';
			}
			else if (!IsVariableFriendly(filename[i]))
			{
				filename[i] = '_';
			}
		}

		return filename;
	}

	std::string ScriptParser::GenerateHeaderFile()
	{
		std::ostringstream file;
		file << "#pragma once\n\n";
		file << "#define ENTT_STANDARD_CPP\n";
		file << "#include <entt/core/hashed_string.hpp>\n";
		file << "#include <entt/core/type_info.hpp>\n";
		file << "#include <entt/meta/factory.hpp>\n";
		file << "#include <entt/meta/meta.hpp>\n";
		file << "#include <entt/meta/resolve.hpp>\n\n";

		file << "#include <nlohmann/json.hpp>\n";
		file << "#include <imgui.h>\n";
		file << "#include <map>\n\n";
		file << "#include \"ImGuiExtended.h\"\n";
		file << "#include \"../" << NCPath::GetFilenameWithoutExt(m_FullFilepath) << ".h\"\n\n";

		file << "namespace Cocoa\n{\n";
		file << "\tnamespace Reflect" << GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(m_FullFilepath)) << " \n\t{\n";

		file << "\t\tusing namespace entt::literals;\n";
		file << "\t\tbool initialized = false;\n\n";

		// Append ids as entt hash strings
		file << "\t\tstd::vector<entt::id_type> ids = \n\t\t{\n";

		int i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1 && m_Classes.size() == 0)
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			int j = 0;
			for (auto uvar : uclass.m_Variables)
			{
				if (i == m_Classes.size() - 1 && j == uclass.m_Variables.size() - 1)
					file << "\t\t\t\"" << uclass.m_ClassName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << uclass.m_ClassName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// Append debug names map
		file << "\t\tstd::map<entt::id_type, const char*> debugNames;\n";

		// Append stringToTypeMap
		file << "\t\tstd::map<std::string, entt::id_type> stringToMap;\n";

		// Create Init function ---------------------------------------------------------------
		file << "\t\tvoid Init()\n";
		file << "\t\t{\n";
		file << "\t\t\tif (initialized) return;\n";
		file << "\t\t\tinitialized = true;\n\n";

		// Debug Names
		file << "\t\tdebugNames = \n\t\t{ \n";

		int id = 0;
		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			file << "\t\t\t{ entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value(), \"" << ustruct.m_StructName.c_str() << "\"},\n";
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1 && m_Classes.size() == 0)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"},\n";
				id++;
				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			int j = 0;
			file << "\t\t\t{ entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value(), \"" << uclass.m_ClassName.c_str() << "\"},\n";
			for (auto uvar : uclass.m_Variables)
			{
				if (i == m_Classes.size() - 1 && j == uclass.m_Variables.size() - 1)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"},\n";

				id++;
				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// String to map
		file << "\t\tstringToMap = \n\t\t{ \n";

		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			if (i == m_Structs.size() - 1 && m_Classes.size() == 0)
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value() },\n";
			// TODO: Create variable string to type id mapper...
			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == m_Classes.size() - 1)
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value() },\n";

			i++;
		}

		file << "\t\t};\n\n";

		// Meta initialization
		id = 0;
		for (auto ustruct : m_Structs)
		{
			std::string camelCaseStructName = ustruct.m_StructName;
			camelCaseStructName[0] = tolower(camelCaseStructName[0]);
			file << "\t\t\tauto " << camelCaseStructName.c_str() << "Factory = entt::meta<" << ustruct.m_StructName.c_str() << ">()\n";

			for (auto var : ustruct.m_Variables)
			{
				file << "\t\t\t\t.data<&" << ustruct.m_StructName.c_str() << "::" << var.m_Identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}

		for (auto uclass : m_Classes)
		{
			std::string camelCaseClassName = uclass.m_ClassName;
			camelCaseClassName[0] = tolower(camelCaseClassName[0]);
			file << "\t\t\tauto " << camelCaseClassName.c_str() << "Factory = entt::meta<" << uclass.m_ClassName.c_str() << ">()\n";

			for (auto var : uclass.m_Variables)
			{
				file << "\t\t\t\t.data<&" << uclass.m_ClassName.c_str() << "::" << var.m_Identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}
		file << "\t\t}\n";

		// Create AddComponent function
		file << "\t\tvoid AddComponent(std::string className, entt::entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";
			file << "(className == \"" << uclass.m_ClassName.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\tEntity e = NEntity::CreateEntity(entity);\n";
			file << "\t\t\t\tNEntity::AddComponent<" << uclass.m_ClassName.c_str() << ">(e);\n";
			file << "\t\t\t}\n";
			i++;
		}
		file << "\t\t}\n";

		// SaveScript function
		file << "\n"
			"		void SaveScript(entt::meta_any any, json& j, Entity entity)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			\n"
			"			int size = j[\"Components\"].size();\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"			json compJson;\n"
			"			compJson[typeName] = {};\n"
			"			compJson[typeName][\"Entity\"] = NEntity::GetID(entity);\n"
			"\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"\n"
			"				if (data.type().is_floating_point())\n"
			"				{\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					float val = data.get(handle).cast<float>();\n"
			"					compJson[typeName][name->second] = val;\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					int val = data.get(handle).cast<int>();\n"
			"					compJson[typeName][name->second] = val;\n"
			"				}\n"
			"			}\n"
			"\n"
			"			j[\"Components\"][size] = compJson;\n"
			"		}\n"
			"\n";

		// Save Scripts function
		file << "\t\tvoid SaveScripts(json& j, entt::registry& registry, SceneData* sceneData)\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tauto view = registry.view<" << uclass.m_ClassName.c_str() << ">();\n";
			file << "\t\t\t\tfor (auto entity : view)\n";
			file << "\t\t\t\t{\n";
			file << "\t\t\t\t\tauto comp = NEntity::GetComponent<" << uclass.m_ClassName.c_str() << ">(NEntity::CreateEntity(entity));\n";
			file << "\t\t\t\t\tentt::meta_any any = { comp };\n";
			file << "\t\t\t\t\tSaveScript(any, j, Entity{ entity });\n";
			file << "\t\t\t\t}\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Load Script function
		file << "\n"
			"		void LoadScript(entt::meta_any any, entt::meta_handle handle, const json& j)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"\n"
			"				if (data.type().is_floating_point())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.set<float>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.set<int>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"			}\n"
			"		}\n"
			"\n";

		// Try Load Script function
		file << "\t\tvoid TryLoad(const json& j, Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tjson::const_iterator it = j.begin();\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e))\n";
		file << "\t\t\t{\n";
		file << "\t\t\t\tLog::Error(\"Invalid entity, entity does not exist.\");\n";
		file << "\t\t\t\treturn;\n";
		file << "\t\t\t}\n";
		file << "\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";

			file << " (it.key() == \"" << uclass.m_ClassName.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = NEntity::AddComponent<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t\tLoadScript({ comp }, comp, j);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// ImGuiAny function
		file << "\n"
			"		void ImGuiAny(entt::meta_any any, entt::meta_handle handle)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			if (ImGui::CollapsingHeader(typeName))\n"
			"			{\n"
			"				CImGui::BeginCollapsingHeaderGroup();\n"
			"				for (auto data : typeData.data())\n"
			"				{\n"
			"					auto name = debugNames.find(data.id());\n"
			"					auto type = debugNames.find(data.type().id());\n"
			"					if (name == debugNames.end() && type == debugNames.end())\n"
			"						continue;\n"
			"\n"
			"					if (data.type().is_floating_point())\n"
			"					{\n"
			"						float* val = (float*)data.get(handle).data();\n"
			"						CImGui::UndoableDragFloat(name->second, *val);\n"
			"					}\n"
			"					else if (data.type().is_integral())\n"
			"					{\n"
			"						int* val = (int*)data.get(handle).data();\n"
			"						CImGui::UndoableDragInt(name->second, *val);\n"
			"					}\n"
			"				}\n"
			"				CImGui::EndCollapsingHeaderGroup();\n"
			"			}\n"
			"		}\n"
			"\n";

		// ImGui function
		file << "\t\tvoid ImGui(Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e)) return;\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			file << "\t\t\tif (NEntity::HasComponent<" << uclass.m_ClassName.c_str() << ">(entity))\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = NEntity::GetComponent<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t\tImGuiAny({ comp }, comp);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// Delete Scripts function
		file << "\t\tvoid DeleteScripts()\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tNEntity::Clear<" << uclass.m_ClassName.c_str() << ">();\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Tabs function
		file << "\n"
			"		int tabs = 0;\n"
			"		void printTabs()\n"
			"		{\n"
			"			for (int i = 0; i < tabs; i++)\n"
			"			{\n"
			"				printf(\"\\t\");\n"
			"			}\n"
			"		}\n"
			"\n";

		// Debug printAny function
		file << "\n"
			"		void debugPrintAny(entt::meta_any any)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"		\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				tabs++;\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"		\n"
			"				if (data.type().is_class())\n"
			"				{\n"
			"					printTabs();\n"
			"					printf(\"%s<%s>\\n\", name->second, type->second);\n"
			"					tabs++;\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					entt::meta_any resolvedData = data.get(handle);\n"
			"					debugPrintAny(resolvedData);\n"
			"					tabs--;\n"
			"				}\n"
			"				else\n"
			"				{\n"
			"					if (data.type().is_floating_point())\n"
			"					{\n"
			"						printTabs();\n"
			"						entt::meta_handle handle = entt::meta_handle(any);\n"
			"						float val = data.get(handle).cast<float>();\n"
			"						printf(\"%s<float>: %2.3f\\n\", name->second, val);\n"
			"					}\n"
			"		 			else if (data.type().is_integral())\n"
			"					{\n"
			"						printTabs();\n"
			"						entt::meta_handle handle = entt::meta_handle(any);\n"
			"						int val = data.get(handle).cast<int>();\n"
			"						printf(\"%s<int>: %d\\n\", name->second, val);\n"
			"					}\n"
			"				}\n"
			"				tabs--;\n"
			"			}\n"
			"		}\n";

		// Begin template print functions
		file << "\t\ttemplate<typename T>\n";
		file << "\t\tvoid debugPrint(const T& obj)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::meta_any any = entt::meta_any{ obj };\n";
		file << "\t\t\tdebugPrintAny(any);\n";
		file << "\t\t}\n";

		// End namespace
		file << "\t}\n";

		// End namespace
		file << "}\n";

		return file.str();
	}

	void ScriptParser::DebugPrint()
	{
		for (auto structIter = m_Structs.begin(); structIter != m_Structs.end(); structIter++)
		{
			Log::Info("%s {", structIter->m_StructName.c_str());
			for (auto varIter = structIter->m_Variables.begin(); varIter != structIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}

		for (auto classIter = m_Classes.begin(); classIter != m_Classes.end(); classIter++)
		{
			Log::Info("%s {", classIter->m_ClassName.c_str());
			for (auto varIter = classIter->m_Variables.begin(); varIter != classIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}
	}

	// Translation Unit
	static AstNode* ParseTranslationUnit();

	// Expressions
	static AstNode* ParsePrimaryExpression();
	static AstNode* ParseIdExpression();
	static AstNode* ParseUnqualifiedId();
	static AstNode* ParseQualifiedId();
	static AstNode* ParseNestedNameSpecifier();

	// Lambdas
	static AstNode* ParseLambdaExpression();
	static AstNode* ParseLambdaIntroducer();
	static AstNode* ParseLambdaCapture();
	static AstNode* ParseCaptureList();
	static AstNode* ParseCapture();
	static AstNode* ParseLambdaDeclarator();

	// Postfix Expressions
	static AstNode* ParsePostfixExpression();
	static AstNode* ParseExpressionList();
	static AstNode* ParsePseudoDestructorName();

	// Unary Expressions
	static AstNode* ParseUnaryExpression();

	// New Expressions
	static AstNode* ParseNewExpression();
	static AstNode* ParseNewPlacement();
	static AstNode* ParseNewTypeId();
	static AstNode* ParseNewDeclarator();
	static AstNode* ParseNoptrNewDeclarator();
	static AstNode* ParseNewInitializer();

	// Delete 
	static AstNode* ParseDeleteExpression();

	// Noexcept
	static AstNode* ParseNoexceptExpression();

	// Cast
	static AstNode* ParseCastExpression();

	// Pointer to member
	static AstNode* ParsePmExpression();

	// Primary operations
	static AstNode* ParseMultiplicativeExpression();
	static AstNode* ParseAdditiveExpression();
	static AstNode* ParseShiftExpression();

	// Comparision operations
	static AstNode* ParseRelationalExpression();
	static AstNode* ParseEqualityExpression();
	static AstNode* ParseAndExpression();

	// Logical operations
	static AstNode* ParseExclusiveOrExpression();
	static AstNode* ParseInclusiveOrExpression();
	static AstNode* ParseLogicalAndExpression();
	static AstNode* ParseLogicalOrExpression();

	// Misc expressions
	static AstNode* ParseConditionalExpression();
	static AstNode* ParseAssignmentExpression();
	static AstNode* ParseAlignmentExpression();

	static AstNode* ParseExpression();
	static AstNode* ParseConstantExpression();

	// Statements
	static AstNode* ParseStatement();
	static AstNode* ParseLabeledStatement();
	static AstNode* ParseExpressionStatement();
	static AstNode* ParseCompoundStatement();
	static AstNode* ParseStatementSequence();

	// Selection statements
	static AstNode* ParseSelectionStatement();
	static AstNode* ParseCondition();

	// Iteration statements
	static AstNode* ParseIterationStatement();
	static AstNode* ParseForInitStatement();
	static AstNode* ParseForRangeDeclaration();
	static AstNode* ParseForRangeInitializer();

	// Jump statements
	static AstNode* ParseJumpStatement();

	// Declarations
	static AstNode* ParseDeclarationStatement();
	static AstNode* ParseDeclarationSequence();
	static AstNode* ParseDeclaration();
	static AstNode* ParseBlockDeclaration();
	static AstNode* ParseAliasDeclaration();
	static AstNode* ParseSimpleDeclaration();
	static AstNode* ParseStaticAssertDeclaration();
	static AstNode* ParseEmptyDeclaration();
	static AstNode* ParseAttributeDeclaration();

	static AstNode* ParseDeclarationSpecifier();
	static AstNode* ParseDeclarationSpecifierSequence();
	static AstNode* ParseStorageClassSpecifier();
	static AstNode* ParseFunctionSpecifier();

	// Types/typedefs
	static AstNode* ParseTypedefName();
	static AstNode* ParseTypeSpecifier();
	static AstNode* ParseTrailingTypeSpecifier();
	static AstNode* ParseTypeSpecifierSequence();
	static AstNode* ParseTrailingTypeSpecifierSequence();

	static AstNode* ParseSimpleTypeSpecifier();
	static AstNode* ParseTypeName();
	static AstNode* ParseDecltypeSpecifier();
	static AstNode* ParseElaboratedTypeSpecifier();

	// Enums
	static AstNode* ParseEnumName();
	static AstNode* ParseEnumSpecifier();
	static AstNode* ParseEnumHead();
	static AstNode* ParseOpaqueEnumDeclaration();
	static AstNode* ParseEnumKey();
	static AstNode* ParseEnumBase();
	static AstNode* ParseEnumeratorList();
	static AstNode* ParseEnumeratorDefinition();

	// Namespaces
	static AstNode* ParseNamespaceName();
	static AstNode* ParseNamespaceDefinition();
	static AstNode* ParseNamedNamespaceDefinition();
	static AstNode* ParseUnnamedNamespaceDefinition();
	static AstNode* ParseNamespaceBody();

	// Namespace alias
	static AstNode* ParseNamespaceAliasDefinition();
	static AstNode* ParseQualifiedNamespaceSpecifier();

	// Using
	static AstNode* ParseUsingDeclaration();
	static AstNode* ParseUsingDirective();
	static AstNode* ParseAsmDefinition();
	static AstNode* ParseLinkageSpecification();

	// Declaration Grammar
	static AstNode* ParseAttributeSpecifierSequence();
	static AstNode* ParseAttributeSpecifier();
	static AstNode* ParseAlignmentSpecifier();
	static AstNode* ParseAttributeList();
	static AstNode* ParseAttribute();
	static AstNode* ParseAttributeToken();
	static AstNode* ParseAttributeArgumentClause();
	static AstNode* ParseBalancedTokenSequence();
	static AstNode* ParseBalancedToken();

	// Declarations
	static AstNode* ParseInitDeclaratorList();
	static AstNode* ParseInitDeclarator();
	static AstNode* ParseDeclarator();
	static AstNode* ParsePtrDeclarator();
	static AstNode* ParseNoPtrDeclarator();
	static AstNode* ParseParametersAndQualifiers();
	static AstNode* ParseTrailingReturnType();
	static AstNode* ParsePtrOperator();
	static AstNode* ParseCvQualifierSequence();
	static AstNode* ParseCvQualifier();
	static AstNode* ParseRefQualifier();
	static AstNode* ParseDeclaratorId();

	// dcl.name
	static AstNode* ParseTypeId();
	static AstNode* ParseAbstractDeclarator();
	static AstNode* ParsePtrAbstractDeclarator();
	static AstNode* ParseNoptrAbstractDeclarator();

	// dcl.fct
	static AstNode* ParseParameterDeclarationClause();
	static AstNode* ParseParameterDeclarationList();
	static AstNode* ParseParameterDeclaration();

	// Functions
	static AstNode* ParseFunctionDefinition();
	static AstNode* ParseFunctionBody();

	// Init
	static AstNode* ParseInitializer();
	static AstNode* ParseBraceOrEqualInitializer();
	static AstNode* ParseInitializerClause();
	static AstNode* ParseInitializerList();
	static AstNode* ParseBracedInitList();

	// Classes
	static AstNode* ParseClassName();
	static AstNode* ParseClassSpecifier();
	static AstNode* ParseClassHead();
	static AstNode* ParseClassHeadName();
	static AstNode* ParseClassVirtSpecifierSequence();
	static AstNode* ParseClassVirtSpecifier();
	static AstNode* ParseClassKey();

	// Class Members
	static AstNode* ParseMemberSpecification();
	static AstNode* ParseMemberDeclaration();
	static AstNode* ParseMemberDeclaratorList();
	static AstNode* ParseMemberDeclarator();
	static AstNode* ParseVirtSpecifierSequence();
	static AstNode* ParseVirtSpecifier();
	static AstNode* ParsePureSpecifier();

	// Derived classes
	static AstNode* ParseBaseClause();
	static AstNode* ParseBaseSpecifierList();
	static AstNode* ParseBaseSpecifier();
	static AstNode* ParseClassOrDecltype();
	static AstNode* ParseBaseTypeSpecifier();
	static AstNode* ParseAccessSpecifier();

	// Class conversion functions
	static AstNode* ParseConversionFunctionId();
	static AstNode* ParseConversionTypeId();
	static AstNode* ParseConversionDeclarator();

	// Class initializers
	static AstNode* ParseCtorInitializer();
	static AstNode* ParseMemInitializerList();
	static AstNode* ParseMemInitializer();
	static AstNode* ParseMemInitializerId();

	// Operator overloading
	static AstNode* ParseOperatorFunctionId();
	static OverloadableOperatorType ParseOverloadableOperator();

	// Literal overrides
	static AstNode* ParseLiteralOperatorId();

	// Templates
	static AstNode* ParseTemplateDeclaration();
	static AstNode* ParseTemplateParameterList();
	static AstNode* ParseTemplateParameter();
	static AstNode* ParseTypeParameter();
	static AstNode* ParseSimpleTemplateId();
	static AstNode* ParseTemplateId();
	static AstNode* ParseTemplateName();
	static AstNode* ParseTemplateArgumentList();
	static AstNode* ParseTemplateArgument();

	static AstNode* ParseTypenameSpecifier();
	static AstNode* ParseExplicitInstantiation();
	static AstNode* ParseExplicitSpecialization();

	// Exceptions
	static AstNode* ParseTryBlock();
	static AstNode* ParseFunctionTryBlock();
	static AstNode* ParseHandlerSequence();
	static AstNode* ParseHandler();
	static AstNode* ParseExceptionDeclaration();

	static AstNode* ParseThrowExpression();
	static AstNode* ParseExceptionSpecification();
	static AstNode* ParseDynamicExceptionSpecification();
	static AstNode* ParseTypeIdList();
	static AstNode* ParseNoexceptSpecification();

	// Preprocessor File
	static AstNode* ParsePreprocessingFile();
	static AstNode* ParseGroup();
	static AstNode* ParseGroupPart();
	static AstNode* ParseIfSection();
	static AstNode* ParseIfGroup();
	static AstNode* ParseElifGroups();
	static AstNode* ParseElifGroup();
	static AstNode* ParseElseGroup();
	static AstNode* ParseControlLine();
	static AstNode* ParseTextLine();
	static AstNode* ParseNonDirective();
	static AstNode* ParseIdentifierList();
	static AstNode* ParseReplacementList();
	static AstNode* ParsePPTokens();
	static AstNode* ParseNumberLiteral();

	// Preprocessor Stuff
	static AstNode* ParsePreprocessingToken();
	static AstNode* ParseHeaderName();
	static AstNode* ParseCharacterLiteral();
	static AstNode* ParseUserDefinedCharacterLiteral();
	static AstNode* ParseStringLiteral();
	static AstNode* ParseUserDefinedStringLiteral();
	static AstNode* ParsePreprocessingOpOrPunc();
	static AstNode* ParseHCharSequence();
	static AstNode* ParseHChar();
	static AstNode* ParseQCharSequence();
	static AstNode* ParseQChar();

	void ScriptParser::Parse()
	{
		AstNode* node = ParseAttributeSpecifier();

		return;
	}

	// Implementation -----------------------------------------------------------------------------------------------------------------------------------------
	// Translation Unit
	static AstNode* ParseTranslationUnit()
	{
		return ParseDeclarationSequence();
	}

	// Expressions
	static AstNode* ParsePrimaryExpression()
	{
		if (Peek() == TokenType::CHARACTER_LITERAL || Peek() == TokenType::FLOATING_POINT_LITERAL || Peek() == TokenType::INTEGER_LITERAL || Peek() == TokenType::STRING_LITERAL)
		{
			return GenerateLiteralNode(ConsumeCurrent(Peek()));
		}

		if (Peek() == TokenType::KW_THIS)
		{
			return GenerateThisNode(ConsumeCurrent(Peek()));
		}

		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GenerateGroupingNode(expression);
		}

		int backtrackPosition = CurrentToken;
		AstNode* expr = ParseIdExpression();
		if (expr->success)
		{
			return expr;
		}
		FreeNode(expr);
		BacktrackTo(backtrackPosition);

		AstNode* lambdaExpr = ParseLambdaExpression();
		if (lambdaExpr->success)
		{
			return lambdaExpr;
		}
		FreeNode(expr);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseIdExpression()
	{
		int backtrackPosition = CurrentToken;
		AstNode* unqualifiedId = ParseUnqualifiedId();
		if (unqualifiedId->success)
		{
			return unqualifiedId;
		}
		FreeNode(unqualifiedId);
		BacktrackTo(backtrackPosition);

		AstNode* qualifiedId = ParseQualifiedId();
		if (qualifiedId->success)
		{
			return qualifiedId;
		}
		FreeNode(qualifiedId);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseUnqualifiedId()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateUnqualifiedIdNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		int backtrackPosition = CurrentToken;
		if (Match(TokenType::TILDE))
		{
			AstNode* className = ParseClassName();
			if (className->success)
			{
				return GenerateUnqualifiedIdDtorClassNode(className);
			}
			FreeNode(className);
			BacktrackTo(backtrackPosition);

			Consume(TokenType::TILDE);
			AstNode* decltypeSpecifier = ParseDecltypeSpecifier();
			if (decltypeSpecifier->success)
			{
				return GenerateUnqualifiedIdDtorDecltypeNode(decltypeSpecifier);
			}
			FreeNode(decltypeSpecifier);
		}
		BacktrackTo(backtrackPosition);

		AstNode* operatorFunctionId = ParseOperatorFunctionId();
		if (operatorFunctionId->success)
		{
			return operatorFunctionId;
		}
		FreeNode(operatorFunctionId);
		BacktrackTo(backtrackPosition);

		AstNode* conversionFunctionId = ParseConversionFunctionId();
		if (conversionFunctionId->success)
		{
			return conversionFunctionId;
		}
		FreeNode(conversionFunctionId);
		BacktrackTo(backtrackPosition);

		AstNode* literalOperatorId = ParseLiteralOperatorId();
		if (literalOperatorId->success)
		{
			return literalOperatorId;
		}
		FreeNode(literalOperatorId);
		BacktrackTo(backtrackPosition);

		AstNode* templateId = ParseTemplateId();
		if (templateId->success)
		{
			return templateId;
		}
		FreeNode(templateId);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseQualifiedId()
	{
		int backtrackPosition = CurrentToken;
		bool hasNamespaceScope = Peek() == TokenType::COLON;
		if (Peek() == TokenType::COLON)
		{
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
		}

		AstNode* nestedNamespaceSpecifier = ParseNestedNameSpecifier();
		bool hasTemplateKeyword = Match(TokenType::KW_TEMPLATE);
		AstNode* unqualifiedId = ParseUnqualifiedId();
		if (!hasNamespaceScope)
		{
			// This can only be a nested-namespace-specifier if it doesn't have a namespace scope
			if (nestedNamespaceSpecifier->success && unqualifiedId->success)
			{
				return GenerateTemplateQualifiedIdNode(nestedNamespaceSpecifier, hasNamespaceScope, hasTemplateKeyword);
			}
			FreeNode(nestedNamespaceSpecifier);
			FreeNode(unqualifiedId);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		if (nestedNamespaceSpecifier->success && unqualifiedId->success)
		{
			return GenerateTemplateQualifiedIdNode(nestedNamespaceSpecifier, hasNamespaceScope, hasTemplateKeyword);
		}
		FreeNode(nestedNamespaceSpecifier);
		FreeNode(unqualifiedId);
		BacktrackTo(backtrackPosition);

		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateQualifiedIdNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		AstNode* operatorFunctionId = ParseOperatorFunctionId();
		if (operatorFunctionId->success)
		{
			return operatorFunctionId;
		}
		FreeNode(operatorFunctionId);
		BacktrackTo(backtrackPosition);

		AstNode* literalOperatorId = ParseLiteralOperatorId();
		if (literalOperatorId->success)
		{
			return literalOperatorId;
		}
		FreeNode(literalOperatorId);
		BacktrackTo(backtrackPosition);

		AstNode* templateId = ParseTemplateId();
		if (templateId->success)
		{
			return templateId;
		}
		FreeNode(templateId);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNestedNameSpecifier()
	{
		int backtrackPosition = CurrentToken;
		AstNode* typeName = ParseTypeName();
		if (typeName->success)
		{
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			return typeName;
		}
		FreeNode(typeName);
		BacktrackTo(backtrackPosition);

		AstNode* namespaceName = ParseNamespaceName();
		if (namespaceName->success)
		{
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			return namespaceName;
		}
		FreeNode(namespaceName);
		BacktrackTo(backtrackPosition);

		AstNode* decltypeSpecifier = ParseDecltypeSpecifier();
		if (decltypeSpecifier->success)
		{
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			return decltypeSpecifier;
		}
		FreeNode(decltypeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		if (Peek() == TokenType::IDENTIFIER)
		{
			Token id = ConsumeCurrent(TokenType::IDENTIFIER);
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			return GenerateNestedNamespaceSpecifierIdNode(nestedNameSpecifier, id);
		}

		bool hasTemplateKeyword = Match(TokenType::KW_TEMPLATE);
		AstNode* simpleTemplateId = ParseSimpleTemplateId();
		if (simpleTemplateId->success)
		{
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			return GenerateNestedNamespaceSpecifierTemplateNode(nestedNameSpecifier, hasTemplateKeyword, simpleTemplateId);
		}
		FreeNode(nestedNameSpecifier);
		FreeNode(simpleTemplateId);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Lambdas
	static AstNode* ParseLambdaExpression()
	{
		int backtrackPosition = CurrentToken;
		AstNode* lambdaIntroducer = ParseLambdaIntroducer();
		if (!lambdaIntroducer->success)
		{
			FreeNode(lambdaIntroducer);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// This is optional, so it's fine if it doesn't succeed
		backtrackPosition = CurrentToken;
		AstNode* lambdaDeclarator = ParseLambdaDeclarator();
		if (!lambdaDeclarator->success)
		{
			BacktrackTo(backtrackPosition);
		}

		backtrackPosition = CurrentToken;
		AstNode* compoundStatement = ParseCompoundStatement();
		if (!compoundStatement->success)
		{
			FreeNode(lambdaIntroducer);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateLambdaExpressionNode(lambdaIntroducer, lambdaDeclarator, compoundStatement);
	}

	static AstNode* ParseLambdaIntroducer()
	{
		if (Match(TokenType::LEFT_BRACKET))
		{
			int backtrackPosition = CurrentToken;
			// Lambda capture is optional, so it's ok if it fails
			AstNode* lambdaCapture = ParseLambdaCapture();
			if (!lambdaCapture->success)
			{
				BacktrackTo(backtrackPosition);
			}
			Consume(TokenType::RIGHT_BRACKET);

			return GenerateLambdaIntroducerNode(lambdaCapture);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseLambdaCapture()
	{
		Token captureDefault;
		captureDefault.m_Type = TokenType::None;
		if (Peek() == TokenType::AND || Peek() == TokenType::EQUAL)
		{
			captureDefault = ConsumeCurrent(Peek());
		}

		if (captureDefault.m_Type != TokenType::None && Match(TokenType::COMMA))
		{
			return GenerateLambdaCaptureNode(captureDefault, ParseCaptureList());
		}

		if (captureDefault.m_Type == TokenType::None)
		{
			return GenerateLambdaCaptureNode(captureDefault, ParseCaptureList());
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseCaptureList()
	{
		AstNode* result = ParseCapture();

		while (Match(TokenType::COMMA))
		{
			result = GenerateLambdaCaptureListNode(result, ParseCaptureList());
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return GenerateLambdaCaptureListNode(result, nullptr);
	}

	static AstNode* ParseCapture()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::AND))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				return GenerateCaptureNode(ConsumeCurrent(TokenType::IDENTIFIER));
			}

			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		if (Peek() == TokenType::KW_THIS)
		{
			return GenerateThisNode(ConsumeCurrent(TokenType::KW_THIS));
		}

		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateCaptureNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseLambdaDeclarator()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* parameterDeclarationClause = ParseParameterDeclarationClause();
			Consume(TokenType::RIGHT_PAREN);

			bool isMutable = Match(TokenType::KW_MUTABLE);

			int backtrackPosition = CurrentToken;
			AstNode* exceptionSpec = ParseExceptionSpecification();
			if (!exceptionSpec->success)
			{
				FreeNode(exceptionSpec);
				exceptionSpec = GenerateNoSuccessAstNode();
				BacktrackTo(backtrackPosition);
			}

			backtrackPosition = CurrentToken;
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			if (!attributeSpecifierSeq->success)
			{
				FreeNode(attributeSpecifierSeq);
				attributeSpecifierSeq = GenerateNoSuccessAstNode();
				BacktrackTo(backtrackPosition);
			}

			backtrackPosition = CurrentToken;
			AstNode* trailingReturnType = ParseTrailingReturnType();
			if (!trailingReturnType->success)
			{
				FreeNode(trailingReturnType);
				trailingReturnType = GenerateNoSuccessAstNode();
				BacktrackTo(backtrackPosition);
			}

			return GenerateLambdaDeclaratorNode(parameterDeclarationClause, isMutable, exceptionSpec, attributeSpecifierSeq, trailingReturnType);
		}

		return GenerateNoSuccessAstNode();
	}

	// Postfix Expressions
	static AstNode* ParsePostfixExpression()
	{
		// TODO: This one scares me test pretty good, otherwise I have a feeling you will get infinite loops which results in horrible lag
		// Try to look ahead and make sure we don't recurse further if it's not possible
		bool shouldRecurse = LookAheadBeforeSemicolon({ TokenType::LEFT_BRACKET, TokenType::LEFT_PAREN, TokenType::DOT, TokenType::ARROW, TokenType::PLUS_PLUS, TokenType::MINUS_MINUS });

		int backtrackPosition = CurrentToken;
		AstNode* primaryExpression = ParsePrimaryExpression();
		if (primaryExpression->success)
		{
			return primaryExpression;
		}
		FreeNode(primaryExpression);
		BacktrackTo(backtrackPosition);

		AstNode* simpleTypeSpecifier = ParseSimpleTypeSpecifier();
		if (simpleTypeSpecifier->success)
		{
			if (Match(TokenType::LEFT_PAREN))
			{
				// Optional
				AstNode* expressionList = ParseExpressionList();
				Consume(TokenType::RIGHT_PAREN);
				return GeneratePostfixSimpleTypeExpressionListNode(simpleTypeSpecifier, expressionList);
			}

			AstNode* bracedInitList = ParseBracedInitList();
			if (!bracedInitList->success)
			{
				FreeNode(simpleTypeSpecifier);
				FreeNode(bracedInitList);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}
			return GeneratePostfixSimpleTypeBraceListNode(simpleTypeSpecifier, bracedInitList);
		}
		FreeNode(simpleTypeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* typenameSpecifier = ParseTypenameSpecifier();
		if (typenameSpecifier->success)
		{
			if (Match(TokenType::LEFT_PAREN))
			{
				// Optional
				AstNode* expressionList = ParseExpressionList();
				Consume(TokenType::RIGHT_PAREN);
				return GeneratePostfixTypenameSpecExpressionListNode(simpleTypeSpecifier, expressionList);
			}

			AstNode* bracedInitList = ParseBracedInitList();
			if (!bracedInitList->success)
			{
				FreeNode(simpleTypeSpecifier);
				FreeNode(bracedInitList);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}
			return GeneratePostfixTypenameSpecBraceListNode(simpleTypeSpecifier, bracedInitList);
		}
		FreeNode(typenameSpecifier);
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::KW_DYNAMIC_CAST))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* typeId = ParseTypeId();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GeneratePostfixCastNode(typeId, expression, CastType::DynamicCast);
		}

		if (Match(TokenType::KW_STATIC_CAST))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* typeId = ParseTypeId();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GeneratePostfixCastNode(typeId, expression, CastType::StaticCast);
		}

		if (Match(TokenType::KW_REINTERPRET_CAST))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* typeId = ParseTypeId();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GeneratePostfixCastNode(typeId, expression, CastType::ReinterpretCast);
		}

		if (Match(TokenType::KW_CONST_CAST))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* typeId = ParseTypeId();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GeneratePostfixCastNode(typeId, expression, CastType::ConstCast);
		}

		if (Match(TokenType::KW_TYPEID))
		{
			Consume(TokenType::LEFT_PAREN);
			int backtrackPos2 = CurrentToken;
			AstNode* expression = ParseExpression();
			if (expression->success)
			{
				Consume(TokenType::RIGHT_PAREN);
				return GeneratePostfixTypeIdExpressionNode(expression);
			}
			FreeNode(expression);
			BacktrackTo(backtrackPos2);

			AstNode* typeId = ParseTypeId();
			if (typeId->success)
			{
				Consume(TokenType::RIGHT_PAREN);
				return GeneratePostfixTypeIdNode(typeId);
			}
			FreeNode(typeId);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		if (!shouldRecurse)
		{
			return GenerateNoSuccessAstNode();
		}

		AstNode* postfixExpression = ParsePostfixExpression();
		if (Match(TokenType::LEFT_BRACKET))
		{
			int backtrackPosition2 = CurrentToken;
			AstNode* expression = ParseExpression();
			if (expression->success)
			{
				Consume(TokenType::RIGHT_BRACKET);
				return GeneratePostfixBracketExpressionNode(postfixExpression, expression);
			}
			FreeNode(expression);
			BacktrackTo(backtrackPosition2);

			// Optional
			AstNode* bracedInitList = ParseBracedInitList();
			Consume(TokenType::RIGHT_BRACKET);
			return GeneratePostfixBracketBraceListNode(postfixExpression, bracedInitList);
		}

		if (Match(TokenType::LEFT_PAREN))
		{
			// Optional
			AstNode* expressionList = ParseExpressionList();
			Consume(TokenType::RIGHT_PAREN);
			return GeneratePostfixParenExpressionListNode(postfixExpression, expressionList);
		}

		bool isDot = Match(TokenType::DOT);
		bool isArrow = Match(TokenType::ARROW);
		if (isDot || isArrow)
		{
			MemberOperatorType memberOp = isDot ? MemberOperatorType::DotOperator : MemberOperatorType::ArrowOperator;
			bool hasTemplateKeyword = Match(TokenType::KW_TEMPLATE);
			if (hasTemplateKeyword)
			{
				AstNode* idExpression = ParseIdExpression();
				if (!idExpression->success)
				{
					FreeNode(postfixExpression);
					return GenerateNoSuccessAstNode();
				}
				return GeneratePostfixMemberIdExpressionNode(postfixExpression, idExpression, hasTemplateKeyword, memberOp);
			}

			int backtrackPosition2 = CurrentToken;
			AstNode* idExpression = ParseIdExpression();
			if (idExpression->success)
			{
				return GeneratePostfixMemberIdExpressionNode(postfixExpression, idExpression, hasTemplateKeyword, memberOp);
			}
			FreeNode(idExpression);
			BacktrackTo(backtrackPosition2);

			AstNode* pseudoDestructorName = ParsePseudoDestructorName();
			return GeneratePostfixPseudoDestructorNode(postfixExpression, pseudoDestructorName, memberOp);
		}

		if (Match(TokenType::PLUS_PLUS))
		{
			return GeneratePostfixPlusPlusNode(postfixExpression);
		}

		if (Match(TokenType::MINUS_MINUS))
		{
			return GeneratePostfixMinusMinusNode(postfixExpression);
		}

		FreeNode(postfixExpression);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExpressionList()
	{
		return ParseInitializerList();
	}

	static AstNode* ParsePseudoDestructorName()
	{
		if (Match(TokenType::TILDE))
		{
			AstNode* decltypeSpecifier = ParseDecltypeSpecifier();
			return GeneratePseudoDestructorDecltypeNode(decltypeSpecifier);
		}

		int backtrackPosition = CurrentToken;
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
		}

		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		if (Match(TokenType::KW_TEMPLATE))
		{
			if (!nestedNameSpecifier->success)
			{
				FreeNode(nestedNameSpecifier);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			AstNode* simpleTemplateId = ParseSimpleTemplateId();
			Consume(TokenType::COLON);
			Consume(TokenType::COLON);
			Consume(TokenType::TILDE);
			AstNode* typeName = ParseTypeName();
			if (!typeName->success)
			{
				FreeNode(nestedNameSpecifier);
				FreeNode(simpleTemplateId);
				FreeNode(typeName);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GeneratePseudoDestructorTemplateNode(nestedNameSpecifier, simpleTemplateId, typeName);
		}

		// Nested name specifier is optional at this point
		if (Match(TokenType::TILDE))
		{
			AstNode* typeName = ParseTypeName();
			if (!typeName->success)
			{
				FreeNode(nestedNameSpecifier);
				FreeNode(typeName);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GeneratePseudoDestructorNode(nestedNameSpecifier, typeName);
		}

		AstNode* nestedTypeName = ParseTypeName();
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
			Consume(TokenType::TILDE);
			AstNode* typeName = ParseTypeName();
			if (!typeName->success)
			{
				FreeNode(nestedNameSpecifier);
				FreeNode(nestedTypeName);
				FreeNode(typeName);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GeneratePseudoNestedDestructorNode(nestedNameSpecifier, nestedTypeName, typeName);
		}

		FreeNode(nestedTypeName);
		FreeNode(nestedNameSpecifier);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Unary Expressions
	static AstNode* ParseUnaryExpression()
	{
		int backtrackCursor = CurrentToken;
		AstNode* postfix = ParsePostfixExpression();
		if (postfix->success)
		{
			return postfix;
		}
		FreeNode(postfix);
		BacktrackTo(backtrackCursor);

		if (Peek() == TokenType::PLUS_PLUS || Peek() == TokenType::MINUS_MINUS || Peek() == TokenType::STAR || Peek() == TokenType::AND ||
			Peek() == TokenType::PLUS || Peek() == TokenType::MINUS || Peek() == TokenType::BANG || Peek() == TokenType::TILDE)
		{
			return GenerateUnaryExpressionNode(ParseOverloadableOperator(), ParseCastExpression());
		}

		if (Match(TokenType::KW_SIZEOF))
		{
			if (Match(TokenType::LEFT_PAREN))
			{
				AstNode* typeId = ParseTypeId();
				Consume(TokenType::RIGHT_PAREN);
				return GenerateSizeofExpressionNode(typeId);
			}

			if (Match(TokenType::DOT))
			{
				Consume(TokenType::DOT);
				Consume(TokenType::DOT);
				Consume(TokenType::LEFT_PAREN);
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				Consume(TokenType::RIGHT_PAREN);
				return GenerateSizeofIdentifierExpressionNode(identifier);
			}

			return GenerateSizeofExpressionNode(ParseUnaryExpression());
		}

		AstNode* alignmentExpression = ParseAlignmentExpression();
		if (alignmentExpression->success)
		{
			return alignmentExpression;
		}
		FreeNode(alignmentExpression);
		BacktrackTo(backtrackCursor);

		AstNode* noexceptExpr = ParseNoexceptExpression();
		if (noexceptExpr->success)
		{
			return noexceptExpr;
		}
		FreeNode(noexceptExpr);
		BacktrackTo(backtrackCursor);

		AstNode* newExpr = ParseNewExpression();
		if (newExpr->success)
		{
			return newExpr;
		}
		FreeNode(newExpr);
		BacktrackTo(backtrackCursor);

		AstNode* deleteExpr = ParseDeleteExpression();
		if (deleteExpr->success)
		{
			return deleteExpr;
		}
		FreeNode(deleteExpr);
		BacktrackTo(backtrackCursor);

		return GenerateNoSuccessAstNode();
	}

	// New Expressions
	static AstNode* ParseNewExpression()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
		}

		if (Match(TokenType::KW_NEW))
		{
			// Optional
			AstNode* newPlacement = ParseNewPlacement();
			if (Match(TokenType::LEFT_PAREN))
			{
				AstNode* typeId = ParseTypeId();
				Consume(TokenType::RIGHT_PAREN);

				// Optional
				AstNode* newInitializer = ParseNewInitializer();
				return GenerateNewExpressionNode(newPlacement, typeId, newInitializer);
			}

			AstNode* newTypeId = ParseNewTypeId();
			if (!newTypeId->success)
			{
				FreeNode(newPlacement);
				FreeNode(newTypeId);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			// Optional
			AstNode* newInitializer = ParseNewInitializer();
			return GenerateNewTypeIdExpressionNode(newPlacement, newTypeId, newInitializer);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNewPlacement()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* expressionList = ParseExpressionList();
			if (!expressionList->success)
			{
				FreeNode(expressionList);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}
			Consume(TokenType::RIGHT_PAREN);

			return GenerateNewPlacementNode(expressionList);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNewTypeId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* typeSpecifierSeq = ParseTypeSpecifierSequence();
		if (!typeSpecifierSeq->success)
		{
			FreeNode(typeSpecifierSeq);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* newDeclarator = ParseNewDeclarator();
		return GenerateNewTypeIdNode(typeSpecifierSeq, newDeclarator);
	}

	static AstNode* ParseNewDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* noptrNewDeclarator = ParseNoptrNewDeclarator();
		if (noptrNewDeclarator->success)
		{
			return noptrNewDeclarator;
		}
		FreeNode(noptrNewDeclarator);
		BacktrackTo(backtrackPosition);

		AstNode* ptrOperator = ParsePtrOperator();
		if (!ptrOperator->success)
		{
			FreeNode(ptrOperator);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* newDeclarator = ParseNewDeclarator();
		return GenerateNewDeclaratorNode(ptrOperator, newDeclarator);
	}

	static AstNode* ParseNoptrNewDeclarator()
	{
		// TODO: this recursion is crazy
		int backtrackPosition = CurrentToken;
		if (!Match(TokenType::LEFT_BRACKET))
		{
			return GenerateNoSuccessAstNode();
		}

		AstNode* expression = ParseExpression();
		if (expression->success)
		{
			Consume(TokenType::RIGHT_BRACKET);

			// Optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			return GenerateNoptrNewTailDeclaratorNode(expression, attributeSpecifierSeq);
		}
		FreeNode(expression);
		BacktrackTo(backtrackPosition);

		Consume(TokenType::LEFT_BRACKET);
		AstNode* constantExpression = ParseConstantExpression();
		if (constantExpression->success)
		{
			Consume(TokenType::RIGHT_BRACKET);
			// Optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();

			AstNode* noptrNewDeclarator = ParseNoptrNewDeclarator();
			if (!noptrNewDeclarator->success)
			{
				FreeNode(noptrNewDeclarator);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GenerateNoptrNewDeclaratorNode(noptrNewDeclarator, constantExpression, attributeSpecifierSeq);
		}

		FreeNode(constantExpression);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNewInitializer()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			// Optional
			AstNode* expressionList = ParseExpressionList();
			Consume(TokenType::RIGHT_PAREN);

			return GenerateNewInitializerNode(expressionList);
		}

		return ParseBracedInitList();
	}

	// Delete 
	static AstNode* ParseDeleteExpression()
	{
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
		}

		if (Match(TokenType::KW_DELETE))
		{
			bool deleteArr = false;
			if (Match(TokenType::LEFT_BRACKET))
			{
				Consume(TokenType::RIGHT_BRACKET);
				deleteArr = true;
			}

			return GenerateDeleteNode(ParseCastExpression(), deleteArr);
		}

		return GenerateNoSuccessAstNode();
	}

	// Noexcept
	static AstNode* ParseNoexceptExpression()
	{
		if (Match(TokenType::KW_NOEXCEPT))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return expression;
		}

		return GenerateNoSuccessAstNode();
	}

	// Cast
	static AstNode* ParseCastExpression()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* typeId = ParseTypeId();
			Consume(TokenType::RIGHT_PAREN);
			return GenerateCastExpressionNode(typeId, ParseCastExpression());
		}

		return ParseUnaryExpression();
	}

	// PointerToMember Expression
	static AstNode* ParsePmExpression()
	{
		AstNode* result = ParseCastExpression();

		// TODO: Does it matter that I'm doing left recursion and he does right recursion????
		while (Match(TokenType::POINTER_TO_MEMBER))
		{
			AstNode* left = ParsePmExpression();
			result = GeneratePointerToMemberNode(left, result);
		}

		return result;
	}

	// Primary operations
	static AstNode* ParseMultiplicativeExpression()
	{
		AstNode* result = ParsePmExpression();

		while (Peek() == TokenType::STAR || Peek() == TokenType::DIV || Peek() == TokenType::MODULO)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseMultiplicativeExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseAdditiveExpression()
	{
		AstNode* result = ParseMultiplicativeExpression();

		while (Peek() == TokenType::PLUS || Peek() == TokenType::MINUS)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseAdditiveExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseShiftExpression()
	{
		AstNode* result = ParseAdditiveExpression();

		while (Peek() == TokenType::LEFT_SHIFT || Peek() == TokenType::RIGHT_SHIFT)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseShiftExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	// Comparision operations
	static AstNode* ParseRelationalExpression()
	{
		AstNode* result = ParseShiftExpression();

		while (Peek() == TokenType::LEFT_ANGLE_BRACKET || Peek() == TokenType::RIGHT_ANGLE_BRACKET || Peek() == TokenType::LESS_THAN_EQ || Peek() == TokenType::GREATER_THAN_EQ)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseRelationalExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseEqualityExpression()
	{
		AstNode* result = ParseRelationalExpression();

		while (Peek() == TokenType::EQUAL_EQUAL || Peek() == TokenType::BANG_EQUAL)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseEqualityExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	// Logical operations
	static AstNode* ParseAndExpression()
	{
		AstNode* result = ParseEqualityExpression();

		while (Peek() == TokenType::AND)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseAndExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseExclusiveOrExpression()
	{
		AstNode* result = ParseAndExpression();

		while (Peek() == TokenType::CARET)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseExclusiveOrExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseInclusiveOrExpression()
	{
		AstNode* result = ParseExclusiveOrExpression();

		while (Peek() == TokenType::PIPE)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseInclusiveOrExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseLogicalAndExpression()
	{
		AstNode* result = ParseInclusiveOrExpression();

		while (Peek() == TokenType::LOGICAL_AND)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseLogicalAndExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	static AstNode* ParseLogicalOrExpression()
	{
		AstNode* result = ParseLogicalAndExpression();

		while (Peek() == TokenType::LOGICAL_OR)
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			AstNode* right = ParseLogicalOrExpression();
			result = GenerateBinaryExpressionNode(result, op, right);
		}

		return result;
	}

	// Misc expressions
	static AstNode* ParseConditionalExpression()
	{
		AstNode* result = ParseLogicalOrExpression();

		if (Match(TokenType::QUESTION))
		{
			AstNode* ifTrueNode = ParseExpression();
			Consume(TokenType::SEMICOLON);
			AstNode* ifFalseNode = ParseAssignmentExpression();
			result = GenerateTernaryExpressionNode(result, ifTrueNode, ifFalseNode);
		}

		return result;
	}

	static AstNode* ParseAssignmentExpression()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseConditionalExpression();

		if (result->success)
		{
			return result;
		}
		FreeNode(result);
		BacktrackTo(backtrackPosition);

		result = ParseLogicalOrExpression();
		if (IsAssignmentOperator(Peek()))
		{
			AssignmentOperatorType assignmentType = AssignmentOperatorType::None;
			switch (Peek())
			{
			case TokenType::EQUAL:
				assignmentType = AssignmentOperatorType::Equal;
				break;
			case TokenType::STAR_EQUAL:
				assignmentType = AssignmentOperatorType::TimesEqual;
				break;
			case TokenType::DIV_EQUAL:
				assignmentType = AssignmentOperatorType::DivEqual;
				break;
			case TokenType::MODULO_EQUAL:
				assignmentType = AssignmentOperatorType::ModEqual;
				break;
			case TokenType::PLUS_EQUAL:
				assignmentType = AssignmentOperatorType::PlusEqual;
				break;
			case TokenType::MINUS_EQUAL:
				assignmentType = AssignmentOperatorType::MinusEqual;
				break;
			case TokenType::RIGHT_SHIFT_EQUAL:
				assignmentType = AssignmentOperatorType::RightShiftEqual;
				break;
			case TokenType::LEFT_SHIFT_EQUAL:
				assignmentType = AssignmentOperatorType::LeftShiftEqual;
				break;
			case TokenType::AND_EQUAL:
				assignmentType = AssignmentOperatorType::AndEqual;
				break;
			case TokenType::CARET_EQUAL:
				assignmentType = AssignmentOperatorType::XorEqual;
				break;
			case TokenType::PIPE_EQUAL:
				assignmentType = AssignmentOperatorType::OrEqual;
				break;
			}
			result = GenerateAssignmentExpressionNode(result, assignmentType, ParseInitializerClause());
			return result;
		}
		FreeNode(result);
		BacktrackTo(backtrackPosition);

		return ParseThrowExpression();
	}

	static AstNode* ParseAlignmentExpression()
	{
		int backtrackPosition = CurrentToken;
		if (Peek() == TokenType::KW_ALIGN_OF)
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* typeId = ParseTypeId();
			if (typeId->success)
			{
				Consume(TokenType::RIGHT_PAREN);
				return GenerateAlignmentExpressionNode(typeId);
			}
			FreeNode(typeId);
			BacktrackTo(backtrackPosition);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExpression()
	{
		AstNode* expression = ParseAssignmentExpression();

		while (Match(TokenType::COMMA))
		{
			AstNode* nextExpression = expression;
			AstNode* expression = GenerateAstNode(AstNodeType::Expression);
			expression->expressionNode.expression = ParseExpression();
			expression->expressionNode.nextExpression = nextExpression;
		}

		return expression;
	}

	static AstNode* ParseConstantExpression()
	{
		return GenerateConstantExpressionNode(ParseConditionalExpression());
	}

	// Statements
	static AstNode* ParseStatement()
	{
		int backtrackPosition = CurrentToken;
		AstNode* labeledStatement = ParseLabeledStatement();
		if (labeledStatement->success)
		{
			return GenerateStatementNode(GenerateNoSuccessAstNode(), labeledStatement);
		}
		FreeNode(labeledStatement);
		BacktrackTo(backtrackPosition);

		AstNode* declarationStatement = ParseDeclarationStatement();
		if (declarationStatement->success)
		{
			return GenerateStatementNode(GenerateNoSuccessAstNode(), declarationStatement);
		}
		FreeNode(declarationStatement);
		BacktrackTo(backtrackPosition);

		// This is optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		int backtrackPosition2 = CurrentToken;

		AstNode* expressionStatement = ParseExpressionStatement();
		if (expressionStatement->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, expressionStatement);
		}
		FreeNode(expressionStatement);
		BacktrackTo(backtrackPosition2);

		AstNode* compoundStatement = ParseCompoundStatement();
		if (compoundStatement->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, compoundStatement);
		}
		FreeNode(compoundStatement);
		BacktrackTo(backtrackPosition2);

		AstNode* selectionStatement = ParseSelectionStatement();
		if (selectionStatement->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, selectionStatement);
		}
		FreeNode(selectionStatement);
		BacktrackTo(backtrackPosition2);

		AstNode* iterationStatement = ParseIterationStatement();
		if (iterationStatement->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, iterationStatement);
		}
		FreeNode(iterationStatement);
		BacktrackTo(backtrackPosition2);

		AstNode* jumpStatement = ParseJumpStatement();
		if (jumpStatement->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, jumpStatement);
		}
		FreeNode(jumpStatement);
		BacktrackTo(backtrackPosition2);

		AstNode* tryBlock = ParseTryBlock();
		if (tryBlock->success)
		{
			return GenerateStatementNode(attributeSpecifierSeq, tryBlock);
		}
		FreeNode(tryBlock);
		FreeNode(attributeSpecifierSeq);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseLabeledStatement()
	{
		int backtrackPosition = CurrentToken;
		// This is optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();

		if (Peek() == TokenType::IDENTIFIER)
		{
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			Consume(TokenType::COLON);
			AstNode* statement = ParseStatement();
			return GenerateLabeledIdentifierNode(attributeSpecifierSeq, identifier, statement);
		}

		if (Match(TokenType::KW_CASE))
		{
			AstNode* constantExpression = ParseConstantExpression();
			Consume(TokenType::COLON);
			AstNode* statement = ParseStatement();
			return GenerateCaseLabelNode(attributeSpecifierSeq, constantExpression, statement);
		}

		if (Match(TokenType::KW_DEFAULT))
		{
			Consume(TokenType::COLON);
			AstNode* statement = ParseStatement();
			return GenerateDefaultLabelNode(attributeSpecifierSeq, statement);
		}

		FreeNode(attributeSpecifierSeq);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExpressionStatement()
	{
		if (Match(TokenType::SEMICOLON))
		{
			return GenerateEmptyStatementNode();
		}

		int backtrackPosition = CurrentToken;
		AstNode* expression = ParseExpression();
		if (expression->success)
		{
			Consume(TokenType::SEMICOLON);
			return expression;
		}

		FreeNode(expression);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseCompoundStatement()
	{
		if (Match(TokenType::LEFT_BRACKET))
		{
			// Optional
			AstNode* statementSequence = ParseStatementSequence();
			Consume(TokenType::RIGHT_BRACKET);
			return GenerateCompoundStatementNode(statementSequence);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseStatementSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseStatement();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		AstNode* nextStatement = nullptr;
		do
		{
			backtrackPosition = CurrentToken;
			AstNode* nextStatement = ParseStatement();
			result = GenerateStatementSequenceNode(result, nextStatement->success ? nextStatement : GenerateNoSuccessAstNode());
		} while (nextStatement && nextStatement->success);

		Log::Assert(nextStatement != nullptr, "Something went horribly wrong when parsing a statement sequence.");
		FreeNode(nextStatement);
		BacktrackTo(backtrackPosition);
		return result;
	}

	// Selection statements
	static AstNode* ParseSelectionStatement()
	{
		if (Match(TokenType::KW_IF))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* condition = ParseCondition();
			Consume(TokenType::RIGHT_PAREN);

			AstNode* ifStatement = ParseStatement();
			AstNode* elseStatement = Match(TokenType::KW_ELSE) ?
				ParseStatement() :
				GenerateNoSuccessAstNode();

			return GenerateIfElseNode(condition, ifStatement, elseStatement);
		}

		if (Match(TokenType::KW_SWITCH))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* condition = ParseCondition();
			Consume(TokenType::RIGHT_PAREN);

			AstNode* statement = ParseStatement();
			return GenerateSwitchNode(condition, statement);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseCondition()
	{
		int backtrackPosition = CurrentToken;
		AstNode* expression = ParseExpression();
		if (expression->success)
		{
			return expression;
		}
		BacktrackTo(backtrackPosition);
		FreeNode(expression);

		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* declSpecifierSeq = ParseDeclarationSpecifierSequence();
		if (!declSpecifierSeq->success)
		{
			BacktrackTo(backtrackPosition);
			FreeNode(attributeSpecifierSeq);
			FreeNode(declSpecifierSeq);
			return GenerateNoSuccessAstNode();
		}

		AstNode* declarator = ParseDeclarator();
		if (Match(TokenType::EQUAL))
		{
			AstNode* initializerClause = ParseInitializerClause();
			return GenerateInitializerConditionNode(attributeSpecifierSeq, declSpecifierSeq, declarator, initializerClause);
		}

		AstNode* bracedInitList = ParseBracedInitList();
		return GenerateBracedInitConditionNode(attributeSpecifierSeq, declSpecifierSeq, declarator, bracedInitList);
	}

	// Iteration statements
	static AstNode* ParseIterationStatement()
	{
		if (Match(TokenType::KW_WHILE))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* condition = ParseCondition();
			Consume(TokenType::RIGHT_PAREN);
			AstNode* statement = ParseStatement();

			return GenerateWhileLoopNode(condition, statement);
		}

		if (Match(TokenType::KW_DO))
		{
			AstNode* statement = ParseStatement();
			Consume(TokenType::KW_WHILE);
			Consume(TokenType::LEFT_PAREN);
			AstNode* condition = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			Consume(TokenType::SEMICOLON);

			return GenerateDoWhileLoopNode(statement, condition);
		}

		if (Match(TokenType::KW_FOR))
		{
			Consume(TokenType::LEFT_PAREN);
			int backtrackPosition = CurrentToken;
			AstNode* forInitStatement = ParseForInitStatement();
			if (forInitStatement->success)
			{
				// This is optional
				AstNode* condition = ParseCondition();
				Consume(TokenType::SEMICOLON);
				// This is also optional
				AstNode* expression = ParseExpression();
				Consume(TokenType::RIGHT_PAREN);
				AstNode* statement = ParseStatement();

				return GenerateForLoopNode(forInitStatement, condition, expression, statement);
			}
			FreeNode(forInitStatement);
			BacktrackTo(backtrackPosition);

			AstNode* forRangeDeclaration = ParseForRangeDeclaration();
			Consume(TokenType::COLON);
			AstNode* forRangeInitializer = ParseForRangeInitializer();
			Consume(TokenType::RIGHT_PAREN);
			AstNode* statement = ParseStatement();

			return GenerateForEachLoopNode(forRangeDeclaration, forRangeInitializer, statement);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseForInitStatement()
	{
		int backtrackPostion = CurrentToken;
		AstNode* expressionStatement = ParseExpressionStatement();
		if (expressionStatement->success)
		{
			return expressionStatement;
		}
		FreeNode(expressionStatement);
		BacktrackTo(backtrackPostion);

		AstNode* simpleDeclaration = ParseSimpleDeclaration();
		if (simpleDeclaration->success)
		{
			return simpleDeclaration;
		}

		FreeNode(simpleDeclaration);
		BacktrackTo(backtrackPostion);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseForRangeDeclaration()
	{
		int backtrackPostion = CurrentToken;
		// This is optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* typeSpecifierSeq = ParseTypeSpecifierSequence();
		if (!typeSpecifierSeq->success)
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(typeSpecifierSeq);
			BacktrackTo(backtrackPostion);
			return GenerateNoSuccessAstNode();
		}

		AstNode* declarator = ParseDeclarator();
		if (!declarator->success)
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(typeSpecifierSeq);
			FreeNode(declarator);
			BacktrackTo(backtrackPostion);
			return GenerateNoSuccessAstNode();
		}

		return GenerateForRangeDeclarationNode(attributeSpecifierSeq, typeSpecifierSeq, declarator);
	}

	static AstNode* ParseForRangeInitializer()
	{
		int backtrackPosition = CurrentToken;
		AstNode* expression = ParseExpression();
		if (!expression->success)
		{
			FreeNode(expression);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// TODO: For each loop seems weird do some good testing on this piece of grammar
		AstNode* bracedInitList = ParseBracedInitList();
		if (!bracedInitList->success)
		{
			FreeNode(expression);
			FreeNode(bracedInitList);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateForRangeInitializerNode(expression, bracedInitList);
	}

	// Jump statements
	static AstNode* ParseJumpStatement()
	{
		if (Match(TokenType::KW_BREAK))
		{
			Consume(TokenType::SEMICOLON);
			return GenerateBreakNode();
		}

		if (Match(TokenType::KW_CONTINUE))
		{
			Consume(TokenType::KW_CONTINUE);
			return GenerateContinueNode();
		}

		if (Match(TokenType::KW_RETURN))
		{
			if (Match(TokenType::SEMICOLON))
			{
				return GenerateReturnNode(GenerateNoSuccessAstNode());
			}

			int backtrackPosition = CurrentToken;
			AstNode* expression = ParseExpression();
			if (expression->success)
			{
				Consume(TokenType::SEMICOLON);
				return GenerateReturnNode(expression);
			}
			FreeNode(expression);
			BacktrackTo(backtrackPosition);

			AstNode* bracedInitList = ParseBracedInitList();
			if (bracedInitList->success)
			{
				Consume(TokenType::SEMICOLON);
				return GenerateReturnNode(bracedInitList);
			}
			FreeNode(bracedInitList);
			BacktrackTo(backtrackPosition);

			return GenerateNoSuccessAstNode();
		}

		if (Match(TokenType::KW_GOTO))
		{
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			Consume(TokenType::SEMICOLON);
			return GenerateGotoNode(identifier);
		}

		return GenerateNoSuccessAstNode();
	}

	// Declarations
	static AstNode* ParseDeclarationStatement()
	{
		return ParseBlockDeclaration();
	}

	static AstNode* ParseDeclarationSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseDeclaration();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextDeclaration = ParseDeclarationSequence();
			result = GenerateDeclarationSeqNode(result, nextDeclaration);
			if (!nextDeclaration->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseDeclaration()
	{
		int backtrackPosition = CurrentToken;
		AstNode* blockDeclaration = ParseBlockDeclaration();
		if (blockDeclaration->success)
		{
			return blockDeclaration;
		}
		FreeNode(blockDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* functionDefinition = ParseFunctionDefinition();
		if (functionDefinition->success)
		{
			return functionDefinition;
		}
		FreeNode(functionDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* templateDeclaration = ParseTemplateDeclaration();
		if (templateDeclaration->success)
		{
			return templateDeclaration;
		}
		FreeNode(templateDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* explicitInstantiation = ParseExplicitInstantiation();
		if (explicitInstantiation->success)
		{
			return explicitInstantiation;
		}
		FreeNode(explicitInstantiation);
		BacktrackTo(backtrackPosition);

		AstNode* explicitSpecialization = ParseExplicitSpecialization();
		if (explicitSpecialization->success)
		{
			return explicitSpecialization;
		}
		FreeNode(explicitSpecialization);
		BacktrackTo(backtrackPosition);

		AstNode* linkageSpecification = ParseLinkageSpecification();
		if (linkageSpecification->success)
		{
			return linkageSpecification;
		}
		FreeNode(linkageSpecification);
		BacktrackTo(backtrackPosition);

		AstNode* namespaceDefinition = ParseNamespaceDefinition();
		if (namespaceDefinition->success)
		{
			return namespaceDefinition;
		}
		FreeNode(namespaceDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* emptyDeclaration = ParseEmptyDeclaration();
		if (emptyDeclaration->success)
		{
			return emptyDeclaration;
		}
		FreeNode(emptyDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* attributeDeclaration = ParseAttributeDeclaration();
		if (attributeDeclaration->success)
		{
			return attributeDeclaration;
		}
		FreeNode(attributeDeclaration);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseBlockDeclaration()
	{
		int backtrackPosition = CurrentToken;
		AstNode* simpleDeclaration = ParseSimpleDeclaration();
		if (simpleDeclaration->success)
		{
			return simpleDeclaration;
		}
		FreeNode(simpleDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* asmDefinition = ParseAsmDefinition();
		if (asmDefinition->success)
		{
			return asmDefinition;
		}
		FreeNode(asmDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* namespaceAliasDefinition = ParseNamespaceAliasDefinition();
		if (namespaceAliasDefinition->success)
		{
			return namespaceAliasDefinition;
		}
		FreeNode(namespaceAliasDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* usingDeclaration = ParseUsingDeclaration();
		if (usingDeclaration->success)
		{
			return usingDeclaration;
		}
		FreeNode(usingDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* usingDirective = ParseUsingDirective();
		if (usingDirective->success)
		{
			return usingDirective;
		}
		FreeNode(usingDirective);
		BacktrackTo(backtrackPosition);

		AstNode* staticAssertDeclaration = ParseStaticAssertDeclaration();
		if (staticAssertDeclaration->success)
		{
			return staticAssertDeclaration;
		}
		FreeNode(staticAssertDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* aliasDeclaration = ParseAliasDeclaration();
		if (aliasDeclaration->success)
		{
			return aliasDeclaration;
		}
		FreeNode(aliasDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* opaqueEnumDeclaration = ParseOpaqueEnumDeclaration();
		if (opaqueEnumDeclaration->success)
		{
			return opaqueEnumDeclaration;
		}
		FreeNode(opaqueEnumDeclaration);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseAliasDeclaration()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_USING))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				Consume(TokenType::EQUAL);
				AstNode* typeId = ParseTypeId();
				if (typeId->success)
				{
					Consume(TokenType::SEMICOLON);
					return GenerateAliasDeclarationNode(identifier, typeId);
				}
				FreeNode(typeId);
			}
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseSimpleDeclaration()
	{
		int backtrackPosition = CurrentToken;
		// All optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* declSpecifierSeq = ParseDeclarationSpecifierSequence();
		AstNode* initDeclaratorList = ParseInitDeclaratorList();
		if (!(attributeSpecifierSeq->success || declSpecifierSeq->success || initDeclaratorList->success))
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(declSpecifierSeq);
			FreeNode(initDeclaratorList);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		Consume(TokenType::SEMICOLON);
		return GenerateSimpleDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, initDeclaratorList);
	}

	static AstNode* ParseStaticAssertDeclaration()
	{
		if (Match(TokenType::KW_STATIC_ASSERT))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* constantExpression = ParseConstantExpression();
			Consume(TokenType::COMMA);
			Token stringLiteral = ConsumeCurrent(TokenType::STRING_LITERAL);
			Consume(TokenType::RIGHT_PAREN);
			Consume(TokenType::SEMICOLON);

			return GenerateStaticAssertDeclarationNode(constantExpression, stringLiteral);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseEmptyDeclaration()
	{
		Consume(TokenType::SEMICOLON);
		AstNode* res = GenerateAstNode(AstNodeType::Empty);
		return res;
	}

	static AstNode* ParseAttributeDeclaration()
	{
		int backtrackPosition = CurrentToken;
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		if (!attributeSpecifierSeq->success)
		{
			FreeNode(attributeSpecifierSeq);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}
		Consume(TokenType::SEMICOLON);

		return attributeSpecifierSeq;
	}

	static AstNode* ParseDeclarationSpecifier()
	{
		if (Peek() == TokenType::KW_FRIEND || Peek() == TokenType::KW_TYPEDEF || Peek() == TokenType::KW_CONST_EXPR)
		{
			return GenerateSimpleDeclSpecifierNode(ConsumeCurrent(Peek()));
		}

		int backtrackPosition = CurrentToken;
		AstNode* storageClassSpecifier = ParseStorageClassSpecifier();
		if (storageClassSpecifier->success)
		{
			return GenerateDeclSpecifierNode(storageClassSpecifier);
		}
		FreeNode(storageClassSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* typeSpecifier = ParseTypeSpecifier();
		if (typeSpecifier->success)
		{
			return GenerateDeclSpecifierNode(typeSpecifier);
		}
		FreeNode(typeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* functionSpecifier = ParseFunctionSpecifier();
		if (functionSpecifier->success)
		{
			return GenerateDeclSpecifierNode(functionSpecifier);
		}
		FreeNode(functionSpecifier);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseDeclarationSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseDeclarationSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextDeclSpec = ParseDeclarationSpecifierSequence();
			result = GenerateDeclSpecSeqNode(result, nextDeclSpec, GenerateNoSuccessAstNode());
			if (!nextDeclSpec->success)
			{
				FreeNode(result->declSpecSeq.attributeSpecifierSeq);
				// Optional
				result->declSpecSeq.attributeSpecifierSeq = ParseAttributeSpecifierSequence();
				break;
			}
		}

		return result;
	}

	static AstNode* ParseStorageClassSpecifier()
	{
		if (PeekIn({ TokenType::KW_AUTO, TokenType::KW_REGISTER, TokenType::KW_STATIC, TokenType::KW_THREAD_LOCAL, TokenType::KW_EXTERN, TokenType::KW_MUTABLE }))
		{
			return GenerateStorageClassSpecNode(ConsumeCurrent(Peek()));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseFunctionSpecifier()
	{
		if (PeekIn({ TokenType::KW_INLINE, TokenType::KW_VIRTUAL, TokenType::KW_EXPLICIT }))
		{
			return GenerateFunctionSpecNode(ConsumeCurrent(Peek()));
		}

		return GenerateNoSuccessAstNode();
	}

	// Types/typedefs
	static AstNode* ParseTypedefName()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateTypedefNameNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypeSpecifier()
	{
		int backtrackPosition = CurrentToken;
		AstNode* trailingTypeSpecifier = ParseTrailingTypeSpecifier();
		if (trailingTypeSpecifier->success)
		{
			return trailingTypeSpecifier;
		}
		FreeNode(trailingTypeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* classSpecifier = ParseClassSpecifier();
		if (classSpecifier->success)
		{
			return classSpecifier;
		}
		FreeNode(classSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* enumSpecifier = ParseEnumSpecifier();
		if (enumSpecifier->success)
		{
			return enumSpecifier;
		}
		FreeNode(enumSpecifier);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTrailingTypeSpecifier()
	{
		int backtrackPosition = CurrentToken;
		AstNode* simpleTypeSpecifier = ParseSimpleTypeSpecifier();
		if (simpleTypeSpecifier->success)
		{
			return simpleTypeSpecifier;
		}
		FreeNode(simpleTypeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* elaboratedTypeSpecifier = ParseElaboratedTypeSpecifier();
		if (elaboratedTypeSpecifier->success)
		{
			return elaboratedTypeSpecifier;
		}
		FreeNode(elaboratedTypeSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* typenameSpecifier = ParseTypenameSpecifier();
		if (typenameSpecifier->success)
		{
			return typenameSpecifier;
		}
		FreeNode(typenameSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* cvQualifier = ParseCvQualifier();
		if (cvQualifier->success)
		{
			return cvQualifier;
		}
		FreeNode(cvQualifier);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypeSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseTypeSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextTypeSpecifier = ParseTypeSpecifierSequence();
			result = GenerateTypeSpecSeqNode(result, nextTypeSpecifier, GenerateNoSuccessAstNode());
			if (!nextTypeSpecifier->success)
			{
				FreeNode(result->typeSpecSeq.attributeSpecifierSeq);
				result->typeSpecSeq.attributeSpecifierSeq = ParseAttributeSpecifierSequence();
				break;
			}
		}

		return result;
	}

	static AstNode* ParseTrailingTypeSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseTrailingTypeSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextTypeSpecifier = ParseTrailingTypeSpecifierSequence();
			result = GenerateTrailingTypeSpecSeqNode(result, nextTypeSpecifier, GenerateNoSuccessAstNode());
			if (!nextTypeSpecifier->success)
			{
				FreeNode(result->trailingTypeSpecSeq.attributeSpecifierSeq);
				result->trailingTypeSpecSeq.attributeSpecifierSeq = ParseAttributeSpecifierSequence();
				break;
			}
		}

		return result;
	}

	static AstNode* ParseSimpleTypeSpecifier()
	{
		if (PeekIn({ TokenType::KW_CHAR, TokenType::KW_CHAR16_T, TokenType::KW_CHAR32_T, TokenType::KW_WCHAR_T, TokenType::KW_BOOL, TokenType::KW_SHORT, TokenType::KW_INT,
			TokenType::KW_LONG, TokenType::KW_SIGNED, TokenType::KW_UNSIGNED, TokenType::KW_FLOAT, TokenType::KW_DOUBLE, TokenType::KW_VOID, TokenType::KW_AUTO }))
		{
			return GenerateSimpleTypeTokenSpecNode(ConsumeCurrent(Peek()));
		}

		int backtrackPosition = CurrentToken;
		AstNode* decltypeSpecifier = ParseDecltypeSpecifier();
		if (decltypeSpecifier->success)
		{
			return decltypeSpecifier;
		}
		FreeNode(decltypeSpecifier);
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
		}

		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		if (Match(TokenType::KW_TEMPLATE))
		{
			if (!nestedNameSpecifier->success)
			{
				FreeNode(nestedNameSpecifier);
				BacktrackTo(backtrackPosition);
			}
			AstNode* simpleTemplateId = ParseSimpleTemplateId();
			if (simpleTemplateId->success)
			{
				return GenerateSimpleTypeTemplateSpecNode(nestedNameSpecifier, simpleTemplateId);
			}
			FreeNode(simpleTemplateId);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		AstNode* typeName = ParseTypeName();
		if (typeName->success)
		{
			return GenerateSimpleTypeSpecNode(nestedNameSpecifier, typeName);
		}

		FreeNode(nestedNameSpecifier);
		FreeNode(typeName);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypeName()
	{
		int backtrackPosition = CurrentToken;
		AstNode* className = ParseClassName();
		if (className->success)
		{
			return className;
		}
		FreeNode(className);
		BacktrackTo(backtrackPosition);

		AstNode* enumName = ParseEnumName();
		if (enumName->success)
		{
			return enumName;
		}
		FreeNode(enumName);
		BacktrackTo(backtrackPosition);

		AstNode* typedefName = ParseTypedefName();
		if (typedefName->success)
		{
			return typedefName;
		}
		FreeNode(typedefName);
		BacktrackTo(backtrackPosition);

		AstNode* simpleTemplateId = ParseSimpleTemplateId();
		if (simpleTemplateId->success)
		{
			return simpleTemplateId;
		}
		FreeNode(simpleTemplateId);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseDecltypeSpecifier()
	{
		if (Match(TokenType::KW_DECLTYPE))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* expression = ParseExpression();
			Consume(TokenType::RIGHT_PAREN);
			return GenerateDecltypeSpecNode(expression);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseElaboratedTypeSpecifier()
	{
		if (Peek() == TokenType::KW_ENUM)
		{
			bool hasScopeOp = Match(TokenType::COLON);
			if (hasScopeOp)
			{
				Consume(TokenType::COLON);
			}

			// Optional
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			return GenerateElaboratedSpecifierEnumNode(nestedNameSpecifier, identifier, hasScopeOp);
		}

		int backtrackPosition = CurrentToken;
		AstNode* classKey = ParseClassKey();
		if (classKey->success)
		{
			int backtrackPosition2 = CurrentToken;
			bool hasScopeOp = Match(TokenType::COLON);
			if (hasScopeOp)
			{
				Consume(TokenType::COLON);
			}

			// TODO: Test if this actually works right...
			bool isTemplate = LookAheadBeforeSemicolon({ TokenType::LEFT_ANGLE_BRACKET });
			if (isTemplate)
			{
				// Optional
				AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
				bool hasTemplateKeyword = Match(TokenType::KW_TEMPLATE);
				AstNode* simpleTemplateId = ParseSimpleTemplateId();
				if (simpleTemplateId->success)
				{
					return GenerateElaboratedSpecifierTemplateNode(classKey, nestedNameSpecifier, simpleTemplateId, hasScopeOp, hasTemplateKeyword);
				}
				FreeNode(simpleTemplateId);
				FreeNode(nestedNameSpecifier);
				BacktrackTo(backtrackPosition2);
			}

			// Optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			hasScopeOp = Match(TokenType::COLON);
			if (hasScopeOp)
			{
				Consume(TokenType::COLON);
			}

			// Optional 
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				return GenerateElaboratedSpecifierClassNode(classKey, attributeSpecifierSeq, nestedNameSpecifier, identifier, hasScopeOp);
			}
			FreeNode(nestedNameSpecifier);
			FreeNode(attributeSpecifierSeq);
		}

		FreeNode(classKey);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Enums
	static AstNode* ParseEnumName()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateEnumNameNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseEnumSpecifier()
	{
		int backtrackPosition = CurrentToken;
		AstNode* enumHead = ParseEnumHead();
		if (enumHead->success)
		{
			Consume(TokenType::LEFT_BRACKET);
			// This is optional it's ok if it fails
			AstNode* enumeratorList = ParseEnumeratorList();
			if (enumeratorList->success)
			{
				// We don't really care about this, but we want to make sure to parse it if it's there
				bool trailingComma = Match(TokenType::COMMA);
			}
			Consume(TokenType::RIGHT_BRACKET);

			return GenerateEnumSpecifierNode(enumHead, enumeratorList);
		}

		FreeNode(enumHead);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseEnumHead()
	{
		int backtrackPosition = CurrentToken;
		AstNode* enumKey = ParseEnumKey();
		if (!enumKey->success)
		{
			FreeNode(enumKey);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// This is optional
		AstNode* attributeSpecifierSequence = ParseAttributeSpecifierSequence();

		backtrackPosition = CurrentToken;
		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		if (nestedNameSpecifier->success)
		{
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			// This is also optional
			AstNode* enumBase = ParseEnumBase();

			return GenerateEnumHeadNode(enumKey, attributeSpecifierSequence, nestedNameSpecifier, identifier, enumBase);
		}

		FreeNode(nestedNameSpecifier);
		BacktrackTo(backtrackPosition);

		Token identifier;
		identifier.m_Type = TokenType::None;
		if (Peek() == TokenType::IDENTIFIER)
		{
			identifier = ConsumeCurrent(TokenType::IDENTIFIER);
		}

		// enum base is optional so this is fine
		return GenerateEnumHeadNode(enumKey, attributeSpecifierSequence, GenerateNoSuccessAstNode(), identifier, ParseEnumBase());
	}

	static AstNode* ParseOpaqueEnumDeclaration()
	{
		int backtrackPosition = CurrentToken;
		AstNode* enumKey = ParseEnumKey();
		if (!enumKey->success)
		{
			BacktrackTo(backtrackPosition);
			FreeNode(enumKey);
			return GenerateNoSuccessAstNode();
		}

		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
		AstNode* enumBase = ParseEnumBase();
		Consume(TokenType::SEMICOLON);

		return GenerateOpaqueEnumDeclNode(enumKey, attributeSpecifierSeq, identifier, enumBase);
	}

	static AstNode* ParseEnumKey()
	{
		if (Match(TokenType::KW_ENUM))
		{
			if (Match(TokenType::KW_CLASS))
			{
				return GenerateEnumKeyNode(EnumKeyType::Class);
			}

			if (Match(TokenType::KW_STRUCT))
			{
				return GenerateEnumKeyNode(EnumKeyType::Struct);
			}

			return GenerateEnumKeyNode(EnumKeyType::Enum);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseEnumBase()
	{
		if (Match(TokenType::COLON))
		{
			AstNode* typeSpecifierSequence = ParseTypeSpecifierSequence();
			return GenerateEnumBaseNode(typeSpecifierSequence);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseEnumeratorList()
	{
		AstNode* result = ParseEnumeratorDefinition();

		while (Match(TokenType::COMMA))
		{
			result = GenerateEnumeratorListNode(result, ParseEnumeratorList());
		}

		return GenerateEnumeratorListNode(result, GenerateNoSuccessAstNode());
	}

	static AstNode* ParseEnumeratorDefinition()
	{
		Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);

		AstNode* constantExpression = Match(TokenType::EQUAL) ?
			ParseConstantExpression() :
			GenerateNoSuccessAstNode();

		return GenerateEnumeratorDefinitionNode(identifier, constantExpression);
	}

	// Namespaces
	static AstNode* ParseNamespaceName()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateNamespaceNameNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNamespaceDefinition()
	{
		int backtrackPosition = CurrentToken;
		AstNode* namedNamespaceDefinition = ParseNamedNamespaceDefinition();
		if (namedNamespaceDefinition->success)
		{
			return namedNamespaceDefinition;
		}
		FreeNode(namedNamespaceDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* unnamedNamespaceDefinition = ParseUnnamedNamespaceDefinition();
		if (unnamedNamespaceDefinition->success)
		{
			return unnamedNamespaceDefinition;
		}
		FreeNode(unnamedNamespaceDefinition);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNamedNamespaceDefinition()
	{
		int backtrackPosition = CurrentToken;
		bool isInline = Match(TokenType::KW_INLINE);
		if (Match(TokenType::KW_NAMESPACE))
		{
			if (!(Peek() == TokenType::IDENTIFIER))
			{
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			Consume(TokenType::LEFT_BRACKET);
			AstNode* namespaceBody = ParseNamespaceBody();
			Consume(TokenType::RIGHT_BRACKET);

			return GenerateNamedNamespaceDefinitionNode(isInline, identifier, namespaceBody);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseUnnamedNamespaceDefinition()
	{
		int backtrackPosition = CurrentToken;
		bool isInline = Match(TokenType::KW_INLINE);
		if (Match(TokenType::KW_NAMESPACE))
		{
			if (Peek() != TokenType::LEFT_BRACKET)
			{
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			Consume(TokenType::LEFT_BRACKET);
			AstNode* namespaceBody = ParseNamespaceBody();
			Consume(TokenType::RIGHT_BRACKET);

			return GenerateUnnamedNamespaceDefinitionNode(isInline, namespaceBody);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNamespaceBody()
	{
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* declarationSequence = ParseDeclarationSequence();
		if (declarationSequence->success)
		{
			return declarationSequence;
		}

		FreeNode(declarationSequence);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Namespace alias
	static AstNode* ParseNamespaceAliasDefinition()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_NAMESPACE))
		{
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			if (!Match(TokenType::EQUAL))
			{
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			AstNode* qualifiedNamespaceSpecifier = ParseQualifiedNamespaceSpecifier();
			Consume(TokenType::SEMICOLON);

			return GenerateNamespaceAliasDefinitionNode(identifier, qualifiedNamespaceSpecifier);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseQualifiedNamespaceSpecifier()
	{
		int backtrackPosition = CurrentToken;

		bool isNested = Match(TokenType::COLON);
		if (isNested) Consume(TokenType::COLON);

		// This is optional
		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		AstNode* namespaceName = ParseNamespaceName();
		if (namespaceName->success)
		{
			return GenerateQualifiedNamespaceSpecifierNode(isNested, nestedNameSpecifier, namespaceName);
		}

		FreeNode(nestedNameSpecifier);
		FreeNode(namespaceName);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Using
	static AstNode* ParseUsingDeclaration()
	{
		if (Match(TokenType::KW_USING))
		{
			if (Match(TokenType::COLON))
			{
				Consume(TokenType::COLON);
				AstNode* unqualifiedId = ParseUnqualifiedId();
				Consume(TokenType::SEMICOLON);
				return GenerateUsingDeclarationNode(unqualifiedId);
			}

			bool isTypename = Match(TokenType::KW_TYPENAME);
			bool isNested = Match(TokenType::COLON);
			if (isNested) Consume(TokenType::COLON);
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			AstNode* unqualifiedId = ParseUnqualifiedId();
			Consume(TokenType::SEMICOLON);

			return GenerateUsingTypenameDeclarationNode(isTypename, isNested, nestedNameSpecifier, unqualifiedId);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseUsingDirective()
	{
		int backtrackPosition = CurrentToken;

		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		if (Match(TokenType::KW_USING))
		{
			bool isNested = Match(TokenType::COLON);
			if (isNested) Match(TokenType::COLON);

			// Optional
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			AstNode* namespaceName = ParseNamespaceName();
			Consume(TokenType::SEMICOLON);

			return GenerateUsingDirectiveNode(attributeSpecifierSeq, isNested, nestedNameSpecifier, namespaceName);
		}

		FreeNode(attributeSpecifierSeq);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseAsmDefinition()
	{
		if (Match(TokenType::KW_ASM))
		{
			Consume(TokenType::LEFT_PAREN);
			Token stringLiteral = ConsumeCurrent(TokenType::STRING_LITERAL);
			Consume(TokenType::RIGHT_PAREN);
			Consume(TokenType::SEMICOLON);

			return GenerateAsmNode(stringLiteral);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseLinkageSpecification()
	{
		if (Match(TokenType::KW_EXTERN))
		{
			Token stringLiteral = ConsumeCurrent(TokenType::STRING_LITERAL);
			if (Match(TokenType::LEFT_BRACKET))
			{
				// Optional
				AstNode* declarationSeq = ParseDeclarationSequence();
				Consume(TokenType::RIGHT_BRACKET);

				return GenerateLinkageSpecificationBlockNode(stringLiteral, declarationSeq);
			}
			else
			{
				AstNode* declaration = ParseDeclaration();
				return GenerateLinkageSpecificationNode(stringLiteral, declaration);
			}
		}

		return GenerateNoSuccessAstNode();
	}

	// Attribute Specifiers
	static AstNode* ParseAttributeSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseAttributeSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextSpec = ParseAttributeSpecifier();
			result = GenerateAttributeSpecifierSequenceNode(result, nextSpec);
			if (!nextSpec->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseAttributeSpecifier()
	{
		if (Match(TokenType::LEFT_BRACKET))
		{
			Consume(TokenType::LEFT_BRACKET);
			AstNode* node = ParseAttributeList();
			Consume(TokenType::RIGHT_BRACKET);
			Consume(TokenType::RIGHT_BRACKET);
			return node;
		}

		return ParseAlignmentSpecifier();
	}

	static AstNode* ParseAlignmentSpecifier()
	{
		if (Match(TokenType::KW_ALIGN_AS))
		{
			Consume(TokenType::LEFT_PAREN);
			int backtrackPosition = CurrentToken;
			AstNode* typeId = ParseTypeId();
			if (typeId->success)
			{
				bool hasElipsis = Match(TokenType::DOT);
				if (hasElipsis)
				{
					Consume(TokenType::DOT); Consume(TokenType::DOT);
				}
				Consume(TokenType::RIGHT_PAREN);

				return GenerateAlignAsTypeIdNode(typeId, hasElipsis);
			}
			FreeNode(typeId);
			BacktrackTo(backtrackPosition);

			AstNode* alignmentExpression = ParseAlignmentExpression();
			bool hasElipsis = Match(TokenType::DOT);
			if (hasElipsis)
			{
				Consume(TokenType::DOT); Consume(TokenType::DOT);
			}
			Consume(TokenType::RIGHT_PAREN);

			return GenerateAlignAsExpressionNode(alignmentExpression, hasElipsis);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseAttributeList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseAttribute();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);

			return GenerateEmptyAttributeListNode();
		}

		while (Match(TokenType::COMMA))
		{
			result = GenerateAttributeListNode(result, ParseAttributeList());
		}

		bool trailingComma = Match(TokenType::COMMA);
		bool elipsis = false;
		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT); Consume(TokenType::DOT);
			elipsis = true;
			if (trailingComma)
			{
				ErrorAtToken();
			}
		}

		return result;
	}

	static AstNode* ParseAttribute()
	{
		int backtrackPosition = CurrentToken;
		AstNode* attributeToken = ParseAttributeToken();
		if (!attributeToken->success)
		{
			FreeNode(attributeToken);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}
		// Optional
		AstNode* attributeArgumentClause = ParseAttributeArgumentClause();
		return GenerateAttributeNode(attributeToken, attributeArgumentClause);
	}

	static AstNode* ParseAttributeToken()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			Token id1 = ConsumeCurrent(TokenType::IDENTIFIER);
			if (Match(TokenType::COLON))
			{
				Consume(TokenType::COLON);
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				return GenerateAttributeTokenNode(id1, identifier);
			}

			Token empty;
			empty.m_Type = TokenType::None;
			return GenerateAttributeTokenNode(empty, id1);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseAttributeArgumentClause()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* balancedTokenSeq = ParseBalancedTokenSequence();
			Consume(TokenType::RIGHT_PAREN);

			return GenerateAttributeArgumentClauseNode(balancedTokenSeq);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseBalancedTokenSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseBalancedToken();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextBalancedToken = ParseBalancedTokenSequence();
			result = GenerateBalancedTokenSeqNode(result, nextBalancedToken);
			if (!nextBalancedToken->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseBalancedToken()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* result = ParseBalancedTokenSequence();
			Consume(TokenType::RIGHT_PAREN);
			return result;
		}

		if (Match(TokenType::LEFT_BRACKET))
		{
			AstNode* result = ParseBalancedTokenSequence();
			Consume(TokenType::RIGHT_BRACKET);
			return result;
		}

		if (Match(TokenType::LEFT_CURLY_BRACKET))
		{
			AstNode* result = ParseBalancedTokenSequence();
			Consume(TokenType::RIGHT_CURLY_BRACKET);
			return result;
		}

		return GenerateBalancedTokenNode(ConsumeCurrent(Peek()));
	}

	// Declarations
	static AstNode* ParseInitDeclaratorList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseInitDeclarator();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextDeclarator = ParseInitDeclaratorList();
			result = GenerateInitDeclaratorListNode(result, nextDeclarator);
			if (!nextDeclarator->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseInitDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* declarator = ParseDeclarator();
		if (!declarator->success)
		{
			FreeNode(declarator);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* initializer = ParseInitializer();

		return GenerateInitDeclaratorNode(declarator, initializer);
	}

	static AstNode* ParseDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* noPtrDeclarator = ParseNoPtrDeclarator();
		if (noPtrDeclarator->success)
		{
			AstNode* parametersAndQualifiers = ParseParametersAndQualifiers();
			AstNode* trailingReturnType = ParseTrailingReturnType();
			if (parametersAndQualifiers->success && trailingReturnType->success)
			{
				return GenerateDeclaratorNode(noPtrDeclarator, parametersAndQualifiers, trailingReturnType);
			}
			FreeNode(parametersAndQualifiers);
			FreeNode(trailingReturnType);
		}
		FreeNode(noPtrDeclarator);
		BacktrackTo(backtrackPosition);

		AstNode* ptrDeclarator = ParsePtrDeclarator();
		if (ptrDeclarator->success)
		{
			return ptrDeclarator;
		}

		FreeNode(ptrDeclarator);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParsePtrDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* noPtrDeclarator = ParseNoPtrDeclarator();
		if (noPtrDeclarator->success)
		{
			return noPtrDeclarator;
		}
		FreeNode(noPtrDeclarator);
		BacktrackTo(backtrackPosition);

		AstNode* ptrOperator = ParsePtrOperator();
		AstNode* ptrDeclarator = ParsePtrDeclarator();
		if (ptrOperator->success && ptrDeclarator->success)
		{
			return GeneratePtrDeclaratorNode(ptrOperator, ptrDeclarator);
		}

		FreeNode(ptrOperator);
		FreeNode(ptrDeclarator);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseNoPtrDeclarator()
	{
		// TODO: Not sure if this works right...?
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::LEFT_BRACKET))
		{
			AstNode* constantExpression = ParseConstantExpression();
			if (constantExpression->success)
			{
				Consume(TokenType::RIGHT_BRACKET);
				AstNode* noptrNewDeclarator = ParseNoptrNewDeclarator();
				AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
				return GenerateNoptrConstantDeclaratorNode(noptrNewDeclarator, constantExpression, attributeSpecifierSeq);
			}
			FreeNode(constantExpression);
			BacktrackTo(backtrackPosition);
		}

		if (Match(TokenType::LEFT_BRACKET))
		{
			AstNode* expression = ParseExpression();
			if (expression->success)
			{
				Consume(TokenType::RIGHT_BRACKET);
				AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
				return GenerateNoptrDeclaratorNode(expression, attributeSpecifierSeq);
			}
			FreeNode(expression);
			BacktrackTo(backtrackPosition);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseParametersAndQualifiers()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* parameterDeclarationClause = ParseParameterDeclarationClause();
			Consume(TokenType::RIGHT_PAREN);
			// Optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			// Optional
			AstNode* cvQualifierSeq = ParseCvQualifierSequence();
			// Optional 
			AstNode* refQualifier = ParseRefQualifier();
			// Optional
			AstNode* exceptionSpec = ParseExceptionSpecification();

			return GenerateParametersAndQualifiersNode(parameterDeclarationClause, attributeSpecifierSeq, cvQualifierSeq, refQualifier, exceptionSpec);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTrailingReturnType()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::ARROW))
		{
			AstNode* trailingTypeSpecifierSeq = ParseTrailingTypeSpecifierSequence();
			if (!trailingTypeSpecifierSeq->success)
			{
				FreeNode(trailingTypeSpecifierSeq);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			// Optional
			AstNode* abstractDeclarator = ParseAbstractDeclarator();

			return GenerateTrailingReturnTypeNode(trailingTypeSpecifierSeq, abstractDeclarator);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParsePtrOperator()
	{
		if (Match(TokenType::STAR))
		{
			// Optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			// Optional
			AstNode* cvQualifierSeq = ParseCvQualifierSequence();
			return GeneratePtrStarNode(attributeSpecifierSeq, cvQualifierSeq);
		}

		if (Match(TokenType::AND))
		{
			if (Match(TokenType::AND))
			{
				return GenerateRefRefNode(ParseAttributeSpecifierSequence());
			}

			return GenerateRefNode(ParseAttributeSpecifierSequence());
		}

		int backtrackPosition = CurrentToken;
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			if (!nestedNameSpecifier)
			{
				FreeNode(nestedNameSpecifier);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			Consume(TokenType::STAR);
			// Both optional
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			AstNode* cvQualifierSeq = ParseCvQualifierSequence();
			return GeneratePtrNamespaceStarNode(nestedNameSpecifier, attributeSpecifierSeq, cvQualifierSeq);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseCvQualifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseCvQualifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextQualifier = ParseCvQualifierSequence();
			result = GenerateCvQualifierSeqNode(result, nextQualifier);
			if (!nextQualifier->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseCvQualifier()
	{
		if (Peek() == TokenType::KW_CONST || Peek() == TokenType::KW_VOLATILE)
		{
			Token token = ConsumeCurrent(Peek());
			return GenerateCvQualifierNode(token);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseRefQualifier()
	{
		if (Match(TokenType::AND))
		{
			bool doubleRef = Match(TokenType::AND);
			return GenerateRefQualifierNode(doubleRef);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseDeclaratorId()
	{
		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
			return ParseIdExpression();
		}

		int backtrackPosition = CurrentToken;
		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
			// Optional
			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			AstNode* className = ParseClassName();
			if (!className->success)
			{
				FreeNode(nestedNameSpecifier);
				FreeNode(className);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GenerateDeclaratorIdNode(nestedNameSpecifier, className);
		}

		backtrackPosition = CurrentToken;
		AstNode* idExpression = ParseIdExpression();
		if (idExpression->success)
		{
			return idExpression;
		}
		FreeNode(idExpression);
		BacktrackTo(backtrackPosition);

		// Optional
		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		AstNode* className = ParseClassName();
		if (className->success)
		{
			return GenerateDeclaratorIdNode(nestedNameSpecifier, className);
		}

		FreeNode(nestedNameSpecifier);
		FreeNode(className);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// dcl.name
	static AstNode* ParseTypeId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* typeSpecifierSequence = ParseTypeSpecifierSequence();
		if (!typeSpecifierSequence->success)
		{
			FreeNode(typeSpecifierSequence);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* abstractDeclarator = ParseAbstractDeclarator();
		return GenerateTypeIdNode(typeSpecifierSequence, abstractDeclarator);
	}

	static AstNode* ParseAbstractDeclarator()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::DOT))
		{
			if (Match(TokenType::DOT))
			{
				if (Match(TokenType::DOT))
				{
					return GenerateAbstractElipsisDeclaratorNode();
				}
			}
		}
		BacktrackTo(backtrackPosition);

		// Optional
		AstNode* noptrAbstractDeclarator = ParseNoptrAbstractDeclarator();
		AstNode* parametersAndQualifiers = ParseParametersAndQualifiers();
		if (parametersAndQualifiers->success)
		{
			AstNode* trailingReturnType = ParseTrailingReturnType();
			if (trailingReturnType->success)
			{
				return GenerateAbstractDeclaratorNode(noptrAbstractDeclarator, parametersAndQualifiers, trailingReturnType);
			}
			FreeNode(trailingReturnType);
		}
		FreeNode(noptrAbstractDeclarator);
		FreeNode(parametersAndQualifiers);
		BacktrackTo(backtrackPosition);

		AstNode* ptrAbstractDeclarator = ParsePtrAbstractDeclarator();
		if (ptrAbstractDeclarator->success)
		{
			return ptrAbstractDeclarator;
		}

		FreeNode(ptrAbstractDeclarator);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParsePtrAbstractDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* ptrOperator = ParsePtrOperator();
		if (ptrOperator->success)
		{
			// Optional
			AstNode* ptrAbstractDeclarator = ParsePtrAbstractDeclarator();
			return GeneratePtrAbstractDeclaratorNode(ptrOperator, ptrAbstractDeclarator);
		}
		FreeNode(ptrOperator);
		BacktrackTo(backtrackPosition);

		return ParseNoptrAbstractDeclarator();
	}

	static AstNode* ParseNoptrAbstractDeclarator()
	{
		// TODO: Not sure if this will work right...?
		int backtrackPosition = CurrentToken;
		AstNode* ptrAbstractDeclarator = GenerateNoSuccessAstNode();
		if (Match(TokenType::LEFT_PAREN))
		{
			ptrAbstractDeclarator = ParsePtrAbstractDeclarator();
			if (!ptrAbstractDeclarator->success)
			{
				FreeNode(ptrAbstractDeclarator);
				BacktrackTo(backtrackPosition);
			}
			Consume(TokenType::RIGHT_PAREN);
		}

		if (Match(TokenType::LEFT_BRACKET))
		{
			AstNode* constantExpression = ParseConstantExpression();
			if (!constantExpression->success)
			{
				FreeNode(constantExpression);
				FreeNode(ptrAbstractDeclarator);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}
			AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
			return GenerateNoptrAbstractExpressionDeclaratorNode(ptrAbstractDeclarator, constantExpression, attributeSpecifierSeq, ParseNoptrAbstractDeclarator());
		}

		AstNode* parametersAndQualifiers = ParseParametersAndQualifiers();
		if (parametersAndQualifiers->success)
		{
			return GenerateNoptrAbstractDeclaratorNode(ptrAbstractDeclarator, parametersAndQualifiers, ParseNoptrAbstractDeclarator());
		}

		FreeNode(parametersAndQualifiers);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// dcl.fct
	static AstNode* ParseParameterDeclarationClause()
	{
		// Optional
		AstNode* parameterDeclarationList = ParseParameterDeclarationList();
		if (parameterDeclarationList->success)
		{
			Consume(TokenType::COMMA);
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
			return parameterDeclarationList;
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return parameterDeclarationList;
	}

	static AstNode* ParseParameterDeclarationList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseParameterDeclaration();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (Match(TokenType::COMMA))
		{
			AstNode* nextParameter = ParseParameterDeclarationList();
			result = GenerateParameterDeclarationListNode(result, nextParameter);
		}

		return result;
	}

	static AstNode* ParseParameterDeclaration()
	{
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* declSpecifierSeq = ParseDeclarationSpecifierSequence();
		if (declSpecifierSeq->success)
		{
			int backtrackPosition2 = CurrentToken;
			AstNode* declarator = ParseDeclarator();
			if (declarator->success)
			{
				if (Match(TokenType::EQUAL))
				{
					AstNode* initializerClause = ParseInitializerClause();
					if (!initializerClause->success)
					{
						FreeNode(initializerClause);
						FreeNode(declarator);
						FreeNode(attributeSpecifierSeq);
						FreeNode(declSpecifierSeq);
						BacktrackTo(backtrackPosition);
						return GenerateNoSuccessAstNode();
					}

					return GenerateParameterDefaultDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, declarator, initializerClause);
				}

				return GenerateParameterDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, declarator);
			}
			FreeNode(declarator);
			BacktrackTo(backtrackPosition2);

			// Optional 
			AstNode* abstractDeclarator = ParseAbstractDeclarator();
			if (Match(TokenType::EQUAL))
			{
				AstNode* initializerClause = ParseInitializerClause();
				if (!initializerClause->success)
				{
					FreeNode(initializerClause);
					FreeNode(abstractDeclarator);
					FreeNode(attributeSpecifierSeq);
					FreeNode(declSpecifierSeq);
					BacktrackTo(backtrackPosition);
					return GenerateNoSuccessAstNode();
				}

				return GenerateParameterAbstractDefaultDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, abstractDeclarator, initializerClause);
			}

			return GenerateParameterAbstractDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, abstractDeclarator);
		}

		FreeNode(attributeSpecifierSeq);
		FreeNode(declSpecifierSeq);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Functions
	static AstNode* ParseFunctionDefinition()
	{
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* declSpecifierSeq = ParseDeclarationSpecifierSequence();
		AstNode* declarator = ParseDeclarator();
		if (!declarator->success)
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(declSpecifierSeq);
			FreeNode(declarator);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		if (Match(TokenType::EQUAL))
		{
			if (Match(TokenType::KW_DEFAULT))
			{
				Consume(TokenType::SEMICOLON);
				return GenerateFunctionDefaultDefinitionNode(attributeSpecifierSeq, declSpecifierSeq, declarator, AutoFunctionType::Default);
			}

			if (Match(TokenType::KW_DELETE))
			{
				Consume(TokenType::SEMICOLON);
				return GenerateFunctionDefaultDefinitionNode(attributeSpecifierSeq, declSpecifierSeq, declarator, AutoFunctionType::Delete);
			}

			FreeNode(attributeSpecifierSeq);
			FreeNode(declSpecifierSeq);
			FreeNode(declarator);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		AstNode* functionBody = ParseFunctionBody();
		if (functionBody->success)
		{
			return GenerateFunctionDefinitionNode(attributeSpecifierSeq, declSpecifierSeq, declarator, functionBody);
		}

		FreeNode(functionBody);
		FreeNode(attributeSpecifierSeq);
		FreeNode(declSpecifierSeq);
		FreeNode(declarator);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseFunctionBody()
	{
		int backtrackPosition = CurrentToken;
		AstNode* functionTryBlock = ParseFunctionTryBlock();
		if (functionTryBlock->success)
		{
			return functionTryBlock;
		}
		FreeNode(functionTryBlock);
		BacktrackTo(backtrackPosition);

		// Optional
		AstNode* ctorInitializer = ParseCtorInitializer();
		AstNode* compoundStatement = ParseCompoundStatement();
		if (compoundStatement->success)
		{
			return GenerateFunctionBodyNode(ctorInitializer, compoundStatement);
		}

		FreeNode(ctorInitializer);
		FreeNode(compoundStatement);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Init
	static AstNode* ParseInitializer()
	{
		if (Match(TokenType::LEFT_PAREN))
		{
			AstNode* expressionList = ParseExpressionList();
			Consume(TokenType::RIGHT_PAREN);
			return expressionList;
		}

		return ParseBraceOrEqualInitializer();
	}

	static AstNode* ParseBraceOrEqualInitializer()
	{
		if (Match(TokenType::EQUAL))
		{
			return ParseInitializerClause();
		}

		return ParseBracedInitList();
	}

	static AstNode* ParseInitializerClause()
	{
		AstNode* result = (AstNode*)AllocMem(sizeof(AstNode));
		result->type = AstNodeType::InitializerClause;

		if (Peek() == TokenType::LEFT_CURLY_BRACKET)
		{
			result->initializerClauseNode.clause = ParseBracedInitList();
		}
		else
		{
			result->initializerClauseNode.clause = ParseAssignmentExpression();
		}
		return result;
	}

	static AstNode* ParseInitializerList()
	{
		AstNode* result = GenerateAstNode(AstNodeType::InitializerList);
		result->initializerList.clauses = std::vector<AstNode*>();
		result->initializerList.clauses.push_back(ParseInitializerClause());

		while (Match(TokenType::COMMA))
		{
			result->initializerList.clauses.push_back(ParseInitializerClause());
		}

		if (Match(TokenType::DOT))
		{
			ConsumeCurrent(TokenType::DOT);
			ConsumeCurrent(TokenType::DOT);
		}

		return result;
	}

	static AstNode* ParseBracedInitList()
	{
		AstNode* result = (AstNode*)AllocMem(sizeof(AstNode));
		result->type = AstNodeType::BracedInitList;
		ConsumeCurrent(TokenType::LEFT_CURLY_BRACKET);
		if (!Match(TokenType::RIGHT_CURLY_BRACKET))
		{
			result->bracedInitList.initList = ParseInitializerList();
			if (Peek() == TokenType::COMMA)
			{
				ConsumeCurrent(TokenType::COMMA);
			}
			ConsumeCurrent(TokenType::RIGHT_CURLY_BRACKET);
			return result;
		}

		ConsumeCurrent(TokenType::RIGHT_CURLY_BRACKET);
		return result;
	}

	// Classes
	static AstNode* ParseClassName()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateClassNameNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return ParseSimpleTemplateId();
	}

	static AstNode* ParseClassSpecifier()
	{
		int backtrackPosition = CurrentToken;
		AstNode* classHead = ParseClassHead();
		if (!classHead->success)
		{
			FreeNode(classHead);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		Consume(TokenType::LEFT_CURLY_BRACKET);
		// Optional
		AstNode* memberSpecification = ParseMemberSpecification();
		Consume(TokenType::RIGHT_CURLY_BRACKET);

		return GenerateClassSpecifierNode(classHead, memberSpecification);
	}

	static AstNode* ParseClassHead()
	{
		int backtrackPosition = CurrentToken;
		AstNode* classKey = ParseClassKey();
		if (!classKey->success)
		{
			FreeNode(classKey);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();

		backtrackPosition = CurrentToken;
		AstNode* classHeadName = ParseClassHeadName();
		if (classHeadName->success)
		{
			// Optional
			AstNode* classVirtSpecifierSeq = ParseClassVirtSpecifierSequence();
			// Optional
			AstNode* baseClause = ParseBaseClause();
			return GenerateClassVirtualHeadNode(classKey, attributeSpecifierSeq, classHeadName, classVirtSpecifierSeq, baseClause);
		}
		FreeNode(classHeadName);
		BacktrackTo(backtrackPosition);

		// Optional
		AstNode* baseClause = ParseBaseClause();
		return GenerateClassHeadNode(classKey, attributeSpecifierSeq, baseClause);
	}

	static AstNode* ParseClassHeadName()
	{
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();

		AstNode* className = ParseClassName();
		if (!className->success)
		{
			FreeNode(nestedNameSpecifier);
			FreeNode(className);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateClassHeadNameNode(nestedNameSpecifier, className);
	}

	static AstNode* ParseClassVirtSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseClassVirtSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextSpec = ParseClassVirtSpecifierSequence();
			result = GenerateClassVirtSpecifierSeqNode(result, nextSpec);
			if (!nextSpec->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseClassVirtSpecifier()
	{
		if (Peek() == TokenType::KW_FINAL || Peek() == TokenType::KW_EXPLICIT)
		{
			Token token = ConsumeCurrent(Peek());
			return GenerateClassVirtSpecifierNode(token);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseClassKey()
	{
		if (Peek() == TokenType::KW_CLASS || Peek() == TokenType::KW_STRUCT || Peek() == TokenType::KW_UNION)
		{
			Token token = ConsumeCurrent(Peek());
			return GenerateClassKeyNode(token);
		}

		return GenerateNoSuccessAstNode();
	}

	// Class Members
	static AstNode* ParseMemberSpecification()
	{
		int backtrackPosition = CurrentToken;
		AstNode* accessSpecifier = ParseAccessSpecifier();
		if (accessSpecifier->success)
		{
			Consume(TokenType::COLON);
			// Optional
			AstNode* memberSpecification = ParseMemberSpecification();
			return GenerateMemberAndAccessSpecifierNode(accessSpecifier, memberSpecification);
		}
		FreeNode(accessSpecifier);
		BacktrackTo(backtrackPosition);

		AstNode* memberDeclaration = ParseMemberDeclaration();
		if (memberDeclaration->success)
		{
			// Optional
			AstNode* memberSpecification = ParseMemberSpecification();
			return GenerateMemberSpecifierNode(memberDeclaration, memberSpecification);
		}

		FreeNode(memberDeclaration);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseMemberDeclaration()
	{
		// TODO: Does this really work right...?
		int backtrackPosition = CurrentToken;
		AstNode* functionDefinition = ParseFunctionDefinition();
		if (functionDefinition->success)
		{
			bool trailingSemicolon = Match(TokenType::SEMICOLON);
			return GenerateMemberFunctionDeclarationNode(functionDefinition, trailingSemicolon);
		}
		FreeNode(functionDefinition);
		BacktrackTo(backtrackPosition);

		AstNode* usingDeclaration = ParseUsingDeclaration();
		if (usingDeclaration->success)
		{
			return usingDeclaration;
		}
		FreeNode(usingDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* staticAssertDeclaration = ParseStaticAssertDeclaration();
		if (staticAssertDeclaration->success)
		{
			return staticAssertDeclaration;
		}
		FreeNode(staticAssertDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* templateDeclaration = ParseTemplateDeclaration();
		if (templateDeclaration->success)
		{
			return templateDeclaration;
		}
		FreeNode(templateDeclaration);
		BacktrackTo(backtrackPosition);

		AstNode* aliasDeclaration = ParseAliasDeclaration();
		if (aliasDeclaration->success)
		{
			return aliasDeclaration;
		}
		FreeNode(aliasDeclaration);
		BacktrackTo(backtrackPosition);

		// All optional except semicolon
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* declSpecifierSeq = ParseDeclarationSpecifierSequence();
		AstNode* memberDeclaratorList = ParseMemberDeclaratorList();
		if (!Match(TokenType::SEMICOLON))
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(declSpecifierSeq);
			FreeNode(memberDeclaratorList);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateMemberDeclarationNode(attributeSpecifierSeq, declSpecifierSeq, memberDeclaratorList);
	}

	static AstNode* ParseMemberDeclaratorList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseMemberDeclarator();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextDeclarator = ParseMemberDeclaratorList();
			result = GenerateMemberDeclaratorListNode(result, nextDeclarator);
			if (!nextDeclarator->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseMemberDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* declarator = ParseDeclarator();
		if (declarator->success)
		{
			// Optional
			AstNode* virtSpecifierSeq = ParseVirtSpecifierSequence();
			// Also optional, but there's a chance we could be referring to a different node
			int backtrackPosition2 = CurrentToken;
			AstNode* pureSpecifier = ParsePureSpecifier();
			if (pureSpecifier->success)
			{
				return GenerateMemberDeclaratorPureNode(declarator, virtSpecifierSeq, pureSpecifier);
			}
			FreeNode(pureSpecifier);
			BacktrackTo(backtrackPosition2);
			// Also optional, but this is the fallback if neither succeeds
			AstNode* braceOrEqualInitializer = ParseBraceOrEqualInitializer();
			return GenerateMemberDeclaratorBraceNode(declarator, virtSpecifierSeq, braceOrEqualInitializer);
		}
		FreeNode(declarator);
		BacktrackTo(backtrackPosition);

		// Optional
		Token identifier;
		identifier.m_Type = TokenType::None;
		if (Peek() == TokenType::IDENTIFIER)
		{
			identifier = ConsumeCurrent(TokenType::IDENTIFIER);
		}

		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		// Optional
		AstNode* virtSpecifierSeq = ParseVirtSpecifierSequence();
		if (!Match(TokenType::COLON))
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(virtSpecifierSeq);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		AstNode* constantExpression = ParseConstantExpression();
		if (!constantExpression->success)
		{
			FreeNode(attributeSpecifierSeq);
			FreeNode(virtSpecifierSeq);
			FreeNode(constantExpression);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateMemberDeclaratorNode(identifier, attributeSpecifierSeq, virtSpecifierSeq, constantExpression);
	}

	static AstNode* ParseVirtSpecifierSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseVirtSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextSpec = ParseVirtSpecifier();
			result = GenerateVirtSpecifierSeqNode(result, nextSpec);
			if (!nextSpec->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseVirtSpecifier()
	{
		if (Peek() == TokenType::KW_OVERRIDE || Peek() == TokenType::KW_FINAL || Peek() == TokenType::KW_NEW)
		{
			Token token = ConsumeCurrent(Peek());
			return GenerateVirtSpecifierNode(token);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParsePureSpecifier()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::EQUAL))
		{
			if (Peek() == TokenType::INTEGER_LITERAL)
			{
				Token token = ConsumeCurrent(TokenType::INTEGER_LITERAL);
				if (strcmp(token.m_Lexeme.String, "0") == 0)
				{
					return GeneratePureSpecifierNode();
				}
			}
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Derived classes
	static AstNode* ParseBaseClause()
	{
		if (Match(TokenType::COLON))
		{
			return ParseBaseSpecifierList();
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseBaseSpecifierList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseBaseSpecifier();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (Match(TokenType::COMMA))
		{
			AstNode* nextBase = ParseBaseSpecifierList();
			result = GenerateBaseSpecifierListNode(result, nextBase);
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return result;
	}

	static AstNode* ParseBaseSpecifier()
	{
		// TODO: this is weird, make sure I didn't goof up here
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		bool isVirtual = Match(TokenType::KW_VIRTUAL);

		int backtrackPosition2 = CurrentToken;
		AstNode* accessSpecifier = ParseAccessSpecifier();
		if (!accessSpecifier->success)
		{
			FreeNode(accessSpecifier);
			BacktrackTo(backtrackPosition2);
			accessSpecifier = GenerateNoSuccessAstNode();

			if (!isVirtual)
			{
				isVirtual = Match(TokenType::KW_VIRTUAL);
			}
		}

		AstNode* baseTypeSpecifier = ParseBaseTypeSpecifier();
		if (baseTypeSpecifier->success)
		{
			return GenerateBaseSpecifierNode(attributeSpecifierSeq, isVirtual, accessSpecifier, baseTypeSpecifier);
		}

		FreeNode(baseTypeSpecifier);
		FreeNode(accessSpecifier);
		FreeNode(attributeSpecifierSeq);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseClassOrDecltype()
	{
		int backtrackPosition = CurrentToken;
		AstNode* decltypeSpecifier = ParseDecltypeSpecifier();
		if (decltypeSpecifier->success)
		{
			return decltypeSpecifier;
		}
		FreeNode(decltypeSpecifier);
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::COLON))
		{
			Consume(TokenType::COLON);
		}

		// Optional
		AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
		AstNode* className = ParseClassName();
		if (className->success)
		{
			return GenerateClassOrDecltypeNode(nestedNameSpecifier, className);
		}

		FreeNode(nestedNameSpecifier);
		FreeNode(className);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseBaseTypeSpecifier()
	{
		return ParseClassOrDecltype();
	}

	static AstNode* ParseAccessSpecifier()
	{
		if (Peek() == TokenType::KW_PRIVATE || Peek() == TokenType::KW_PROTECTED || Peek() == TokenType::KW_PUBLIC)
		{
			Token accessSpecifier = ConsumeCurrent(Peek());
			return GenerateAccessSpecifierNode(accessSpecifier);
		}

		return GenerateNoSuccessAstNode();
	}

	// Class conversion functions
	static AstNode* ParseConversionFunctionId()
	{
		if (Match(TokenType::KW_OPERATOR))
		{
			AstNode* conversionTypeId = ParseConversionTypeId();
			return GenerateConversionFunctionIdNode(conversionTypeId);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseConversionTypeId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* typeSpecifierSeq = ParseTypeSpecifierSequence();
		if (!typeSpecifierSeq->success)
		{
			FreeNode(typeSpecifierSeq);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* conversionDeclarator = ParseConversionDeclarator();
		return GenerateConversionTypeIdNode(typeSpecifierSeq, conversionDeclarator);
	}

	static AstNode* ParseConversionDeclarator()
	{
		int backtrackPosition = CurrentToken;
		AstNode* ptrOperator = ParsePtrOperator();
		if (!ptrOperator->success)
		{
			FreeNode(ptrOperator);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		// Optional
		AstNode* conversionDeclarator = ParseConversionDeclarator();
		return GenerateConversionDeclaratorNode(ptrOperator, conversionDeclarator);
	}

	// Class initializers
	static AstNode* ParseCtorInitializer()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::COLON))
		{
			AstNode* memInitializerList = ParseMemInitializerList();
			if (memInitializerList->success)
			{
				return memInitializerList;
			}
			FreeNode(memInitializerList);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseMemInitializerList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseMemInitializer();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (Match(TokenType::COMMA))
		{
			AstNode* nextInitializer = ParseMemInitializer();
			result = GenerateMemInitializerListNode(result, nextInitializer);
			if (!nextInitializer->success)
			{
				break;
			}
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return result;
	}

	static AstNode* ParseMemInitializer()
	{
		int backtrackPosition = CurrentToken;
		AstNode* memInitializerId = ParseMemInitializerId();
		if (!memInitializerId->success)
		{
			FreeNode(memInitializerId);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		if (Match(TokenType::LEFT_PAREN))
		{
			// Optional
			AstNode* expressionList = ParseExpressionList();
			Consume(TokenType::RIGHT_PAREN);

			return GenerateMemExpressionInitializerNode(memInitializerId, expressionList);
		}

		AstNode* bracedInitList = ParseBracedInitList();
		if (!bracedInitList->success)
		{
			FreeNode(memInitializerId);
			FreeNode(bracedInitList);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		return GenerateMemBracedInitializerNode(memInitializerId, bracedInitList);
	}

	static AstNode* ParseMemInitializerId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* classOrDecltype = ParseClassOrDecltype();
		if (classOrDecltype->success)
		{
			return classOrDecltype;
		}
		FreeNode(classOrDecltype);
		BacktrackTo(backtrackPosition);

		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateMemInitializerIdNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	// Operator overloading
	static OverloadableOperatorType ParseOverloadableOperator()
	{
		if (Match(TokenType::KW_NEW))
		{
			if (Match(TokenType::LEFT_BRACKET))
			{
				Consume(TokenType::RIGHT_BRACKET);
				return OverloadableOperatorType::NewArr;
			}
			return  OverloadableOperatorType::New;
		}

		if (Match(TokenType::KW_DELETE))
		{
			if (Match(TokenType::LEFT_BRACKET))
			{
				Consume(TokenType::RIGHT_BRACKET);
				return OverloadableOperatorType::DeleteArr;
			}
			return OverloadableOperatorType::Delete;
		}

		Token token = ConsumeCurrent(Peek());
		switch (token.m_Type)
		{
		case TokenType::PLUS:
			return OverloadableOperatorType::Plus;
		case TokenType::MINUS:
			return OverloadableOperatorType::Minus;
		case TokenType::STAR:
			return OverloadableOperatorType::Multiply;
		case TokenType::DIV:
			return OverloadableOperatorType::Divide;
		case TokenType::MODULO:
			return OverloadableOperatorType::Modulo;
		case TokenType::CARET:
			return OverloadableOperatorType::Xor;
		case TokenType::AND:
			return OverloadableOperatorType::BitAnd;
		case TokenType::PIPE:
			return OverloadableOperatorType::BitOr;
		case TokenType::TILDE:
			return OverloadableOperatorType::BitComplement;
		case TokenType::BANG:
			return OverloadableOperatorType::Not;
		case TokenType::EQUAL:
			return OverloadableOperatorType::Assign;
		case TokenType::LEFT_ANGLE_BRACKET:
			return OverloadableOperatorType::LessThan;
		case TokenType::RIGHT_ANGLE_BRACKET:
			return OverloadableOperatorType::GreaterThan;
		case TokenType::PLUS_EQUAL:
			return OverloadableOperatorType::PlusEqual;
		case TokenType::MINUS_EQUAL:
			return OverloadableOperatorType::MinusEqual;
		case TokenType::STAR_EQUAL:
			return OverloadableOperatorType::MultiplyEqual;
		case TokenType::DIV_EQUAL:
			return OverloadableOperatorType::DivideEqual;
		case TokenType::MODULO_EQUAL:
			return OverloadableOperatorType::ModuloEqual;
		case TokenType::CARET_EQUAL:
			return OverloadableOperatorType::CaretEqual;
		case TokenType::AND_EQUAL:
			return OverloadableOperatorType::BitAndEqual;
		case TokenType::PIPE_EQUAL:
			return OverloadableOperatorType::BitOrEqual;
		case TokenType::LEFT_SHIFT:
			return OverloadableOperatorType::LeftShift;
		case TokenType::RIGHT_SHIFT:
			return OverloadableOperatorType::RightShift;
		case TokenType::RIGHT_SHIFT_EQUAL:
			return OverloadableOperatorType::RightShiftEqual;
		case TokenType::LEFT_SHIFT_EQUAL:
			return OverloadableOperatorType::LeftShiftEqual;
		case TokenType::EQUAL_EQUAL:
			return OverloadableOperatorType::EqualEqual;
		case TokenType::BANG_EQUAL:
			return OverloadableOperatorType::NotEqual;
		case TokenType::LESS_THAN_EQ:
			return OverloadableOperatorType::LessThanEqual;
		case TokenType::GREATER_THAN_EQ:
			return OverloadableOperatorType::GreaterThanEqual;
		case TokenType::LOGICAL_AND:
			return OverloadableOperatorType::LogicAnd;
		case TokenType::LOGICAL_OR:
			return OverloadableOperatorType::LogicOr;
		case TokenType::PLUS_PLUS:
			return OverloadableOperatorType::PlusPlus;
		case TokenType::MINUS_MINUS:
			return OverloadableOperatorType::MinusMinus;
		case TokenType::COMMA:
			return OverloadableOperatorType::Comma;
		case TokenType::ARROW:
		{
			if (Match(TokenType::STAR))
			{
				return OverloadableOperatorType::ArrowStar;
			}
			return OverloadableOperatorType::Arrow;
		}
		case TokenType::LEFT_PAREN:
			Consume(TokenType::RIGHT_PAREN);
			return OverloadableOperatorType::ParenGroup;
		case TokenType::LEFT_BRACKET:
			Consume(TokenType::RIGHT_BRACKET);
			return OverloadableOperatorType::BracketGroup;
		}

		return OverloadableOperatorType::None;
	}

	static AstNode* ParseOperatorFunctionId()
	{
		if (Match(TokenType::KW_OPERATOR))
		{
			OverloadableOperatorType op = ParseOverloadableOperator();
			if (Match(TokenType::LEFT_ANGLE_BRACKET))
			{
				AstNode* templateArgList = ParseTemplateArgumentList();
				Consume(TokenType::RIGHT_ANGLE_BRACKET);
				return GenerateOperatorFunctionIdNode(op, templateArgList);
			}

			return GenerateOperatorFunctionIdNode(op, GenerateNoSuccessAstNode());
		}

		return GenerateNoSuccessAstNode();
	}

	// Literal overrides
	static AstNode* ParseLiteralOperatorId()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_OPERATOR))
		{
			if (Peek() == TokenType::STRING_LITERAL)
			{
				Token token = ConsumeCurrent(TokenType::STRING_LITERAL);
				Log::Assert(token.m_Lexeme.Length == 0, "Invalid custom overloaded operator. Syntax is 'operator\"\" identifier'");
				if (Peek() == TokenType::IDENTIFIER)
				{
					Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
					return GenerateLiteralOperatorIdNode(identifier);
				}
			}
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Templates
	static AstNode* ParseTemplateDeclaration()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_TEMPLATE))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* templateParameterList = ParseTemplateParameterList();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			AstNode* declaration = ParseDeclaration();

			if (!(templateParameterList->success && declaration->success))
			{
				FreeNode(templateParameterList);
				FreeNode(declaration);
				BacktrackTo(backtrackPosition);
				return GenerateNoSuccessAstNode();
			}

			return GenerateTemplateDeclarationNode(templateParameterList, declaration);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTemplateParameterList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseTemplateParameter();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (Match(TokenType::COMMA))
		{
			result = GenerateTemplateParameterListNode(result, ParseTemplateParameterList());
		}

		return GenerateTemplateParameterListNode(result, GenerateNoSuccessAstNode());
	}

	static AstNode* ParseTemplateParameter()
	{
		int backtrackPosition = CurrentToken;
		AstNode* typeParameter = ParseTypeParameter();
		if (typeParameter->success)
		{
			return typeParameter;
		}
		FreeNode(typeParameter);
		BacktrackTo(backtrackPosition);

		AstNode* parameterDeclaration = ParseParameterDeclaration();
		if (parameterDeclaration->success)
		{
			return parameterDeclaration;
		}

		FreeNode(parameterDeclaration);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypeParameter()
	{
		// TODO: I think this guy needs some work...
		AstNode* result = GenerateAstNode(AstNodeType::TypeParameter);
		result->typeParameter.templateParameterList = nullptr;
		if (Match(TokenType::KW_TEMPLATE))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			AstNode* templateParameterList = ParseTemplateParameterList();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			result->typeParameter.templateParameterList = templateParameterList;
		}

		if (Match(TokenType::KW_CLASS))
		{
			if (Match(TokenType::DOT))
			{
				Consume(TokenType::DOT);
				Consume(TokenType::DOT);
				if (Peek() == TokenType::IDENTIFIER)
				{
					result->typeParameter.identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				}
				return result;
			}

			if (Peek() == TokenType::IDENTIFIER)
			{
				result->typeParameter.identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			}

			Consume(TokenType::EQUAL);
			result->typeParameter.typeId = ParseTypeId();
			return result;
		}

		if (result->typeParameter.templateParameterList != nullptr)
		{
			Log::Error("Invlaid template parameter.");
			ErrorAtToken();
		}

		if (Match(TokenType::KW_TYPENAME))
		{
			if (Match(TokenType::DOT))
			{
				Consume(TokenType::DOT);
				Consume(TokenType::DOT);
				if (Peek() == TokenType::IDENTIFIER)
				{
					result->typeParameter.identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				}
				return result;
			}

			if (Peek() == TokenType::IDENTIFIER)
			{
				result->typeParameter.identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			}

			Consume(TokenType::EQUAL);
			result->typeParameter.typeId = ParseTypeId();
			return result;
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseSimpleTemplateId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* templateName = ParseTemplateName();
		if (templateName->success)
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			// Optional
			AstNode* templateArgumentList = ParseTemplateArgumentList();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);

			return GenerateSimpleTemplateIdNode(templateName, templateArgumentList);
		}

		FreeNode(templateName);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTemplateId()
	{
		int backtrackPosition = CurrentToken;
		AstNode* simpleTemplateId = ParseSimpleTemplateId();
		if (simpleTemplateId->success)
		{
			return simpleTemplateId;
		}
		FreeNode(simpleTemplateId);
		BacktrackTo(backtrackPosition);

		AstNode* literalOperatorId = ParseLiteralOperatorId();
		if (literalOperatorId->success)
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			// Optional
			AstNode* templateArgumentList = ParseTemplateArgumentList();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			return GenerateLiteralOperatorTemplateIdNode(literalOperatorId, templateArgumentList);
		}
		FreeNode(literalOperatorId);
		BacktrackTo(backtrackPosition);

		AstNode* operatorFunctionId = ParseOperatorFunctionId();
		if (operatorFunctionId->success)
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			// Optional
			AstNode* templateArgumentList = ParseTemplateArgumentList();
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			return GenerateFunctionOperatorTemplateIdNode(operatorFunctionId, templateArgumentList);
		}
		FreeNode(operatorFunctionId);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTemplateName()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateTemplateNameNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTemplateArgumentList()
	{
		AstNode* result = ParseTemplateArgument();

		while (Match(TokenType::COMMA))
		{
			result = GenerateTemplateArgumentListNode(result, ParseTemplateArgumentList());
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return GenerateTemplateArgumentListNode(result, GenerateNoSuccessAstNode());
	}

	static AstNode* ParseTemplateArgument()
	{
		int backtrackPosition = CurrentToken;
		AstNode* constantExpression = ParseConstantExpression();
		if (constantExpression->success)
		{
			return constantExpression;
		}
		FreeNode(constantExpression);
		BacktrackTo(backtrackPosition);

		AstNode* typeId = ParseTypeId();
		if (typeId->success)
		{
			return typeId;
		}
		FreeNode(typeId);
		BacktrackTo(backtrackPosition);

		AstNode* idExpression = ParseIdExpression();
		if (idExpression->success)
		{
			return idExpression;
		}
		FreeNode(idExpression);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypenameSpecifier()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_TYPENAME))
		{
			if (Match(TokenType::COLON))
			{
				Consume(TokenType::COLON);
			}

			AstNode* nestedNameSpecifier = ParseNestedNameSpecifier();
			if (nestedNameSpecifier->success)
			{
				if (Peek() == TokenType::IDENTIFIER)
				{
					return GenerateTypenameSpecifierNode(nestedNameSpecifier, ConsumeCurrent(TokenType::IDENTIFIER));
				}

				bool hasTemplateKeyword = Match(TokenType::KW_TEMPLATE);
				AstNode* simpleTemplateId = ParseSimpleTemplateId();
				if (simpleTemplateId->success)
				{
					return GenerateTypenameTemplateSpecifierNode(nestedNameSpecifier, simpleTemplateId, hasTemplateKeyword);
				}
				FreeNode(simpleTemplateId);
			}
			FreeNode(nestedNameSpecifier);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExplicitInstantiation()
	{
		int backtrackPosition = CurrentToken;
		bool hasExternKeyword = Match(TokenType::KW_EXTERN);
		if (Match(TokenType::KW_TEMPLATE))
		{
			AstNode* declaration = ParseDeclaration();
			if (declaration->success)
			{
				return GenerateExplicitInstantiationNode(declaration, hasExternKeyword);
			}
			FreeNode(declaration);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExplicitSpecialization()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_TEMPLATE))
		{
			Consume(TokenType::LEFT_ANGLE_BRACKET);
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			AstNode* declaration = ParseDeclaration();
			if (declaration->success)
			{
				declaration;
			}
			FreeNode(declaration);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	// Exceptions
	static AstNode* ParseTryBlock()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_TRY))
		{
			AstNode* compoundStatement = ParseCompoundStatement();
			if (compoundStatement->success)
			{
				AstNode* handlerSeq = ParseHandlerSequence();
				if (handlerSeq->success)
				{
					return GenerateTryBlockNode(compoundStatement, handlerSeq);
				}
				FreeNode(handlerSeq);
			}
			FreeNode(compoundStatement);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseFunctionTryBlock()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_TRY))
		{
			// Optional
			AstNode* ctorInitializer = ParseCtorInitializer();
			AstNode* compoundStatement = ParseCompoundStatement();
			if (compoundStatement->success)
			{
				AstNode* handlerSeq = ParseHandlerSequence();
				if (handlerSeq->success)
				{
					return GenerateFunctionTryBlockNode(ctorInitializer, compoundStatement, handlerSeq);
				}
				FreeNode(handlerSeq);
			}
			FreeNode(compoundStatement);
			FreeNode(ctorInitializer);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseHandlerSequence()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseHandler();
		if (!result->success)
		{
			BacktrackTo(backtrackPosition);
			FreeNode(result);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextHandler = ParseHandlerSequence();
			result = GenerateHandlerSeqNode(result, nextHandler);
			if (!nextHandler->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseHandler()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_CATCH))
		{
			Consume(TokenType::LEFT_PAREN);
			AstNode* exceptionDeclaration = ParseExceptionDeclaration();
			if (exceptionDeclaration->success)
			{
				Consume(TokenType::RIGHT_PAREN);
				AstNode* compoundStatement = ParseCompoundStatement();
				if (compoundStatement->success)
				{
					return GenerateHandlerNode(exceptionDeclaration, compoundStatement);
				}
				FreeNode(compoundStatement);
			}
			FreeNode(exceptionDeclaration);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExceptionDeclaration()
	{
		int backtrackPosition = CurrentToken;
		// Optional
		AstNode* attributeSpecifierSeq = ParseAttributeSpecifierSequence();
		AstNode* typeSpecifierSeq = ParseTypeSpecifierSequence();
		if (typeSpecifierSeq->success)
		{
			int backtrackPosition2 = CurrentToken;
			AstNode* declarator = ParseDeclarator();
			if (declarator->success)
			{
				return GenerateExceptionDeclarationNode(attributeSpecifierSeq, typeSpecifierSeq, declarator);
			}
			FreeNode(declarator);
			BacktrackTo(backtrackPosition2);

			// Optional
			AstNode* abstractDeclarator = ParseAbstractDeclarator();
			return GenerateExceptionAbstractDeclarationNode(attributeSpecifierSeq, typeSpecifierSeq, declarator);
		}

		FreeNode(attributeSpecifierSeq);
		FreeNode(typeSpecifierSeq);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseThrowExpression()
	{
		if (Match(TokenType::KW_THROW))
		{
			// Optional
			AstNode* assignmentExpression = ParseAssignmentExpression();
			return GenerateThrowExpressionNode(assignmentExpression);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseExceptionSpecification()
	{
		int backtrackPosition = CurrentToken;
		AstNode* noexceptSpecification = ParseNoexceptSpecification();
		if (noexceptSpecification->success)
		{
			return noexceptSpecification;
		}
		FreeNode(noexceptSpecification);
		BacktrackTo(backtrackPosition);

		AstNode* dynamicExceptionSpecification = ParseDynamicExceptionSpecification();
		if (dynamicExceptionSpecification->success)
		{
			return dynamicExceptionSpecification;
		}
		FreeNode(dynamicExceptionSpecification);
		BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseDynamicExceptionSpecification()
	{
		if (Match(TokenType::KW_THROW))
		{
			Consume(TokenType::LEFT_PAREN);
			// Optional
			AstNode* typeIdList = ParseTypeIdList();
			Consume(TokenType::RIGHT_PAREN);

			return GenerateDynamicExceptionSpecNode(typeIdList);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTypeIdList()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseTypeId();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (Match(TokenType::COMMA))
		{
			result = GenerateTypeIdListNode(result, ParseTypeIdList());
		}

		if (Match(TokenType::DOT))
		{
			Consume(TokenType::DOT);
			Consume(TokenType::DOT);
		}

		return GenerateTypeIdListNode(result, GenerateNoSuccessAstNode());
	}

	static AstNode* ParseNoexceptSpecification()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::KW_NOEXCEPT))
		{
			if (Match(TokenType::LEFT_PAREN))
			{
				AstNode* constantExpression = ParseConstantExpression();
				if (constantExpression->success)
				{
					Consume(TokenType::RIGHT_PAREN);
					return GenerateNoexceptExpressionSpecNode(constantExpression);
				}
				FreeNode(constantExpression);
				return GenerateNoSuccessAstNode();
			}

			return GenerateNoexceptSpecNode();
		}

		return GenerateNoSuccessAstNode();
	}

	// Preprocessing Stuff
	static AstNode* ParsePreprocessingFile()
	{
		return GeneratePreprocessingFileNode(ParseGroup());
	}

	static AstNode* ParseGroup()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseGroupPart();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
		}

		while (true)
		{
			AstNode* nextGroup = ParseGroup();
			result = GenerateGroupNode(result, nextGroup);
			if (!nextGroup->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseGroupPart()
	{
		int backtrackPosition = CurrentToken;
		AstNode* ifSection = ParseIfSection();
		if (ifSection->success)
		{
			return ifSection;
		}
		FreeNode(ifSection);
		BacktrackTo(backtrackPosition);

		AstNode* controlLine = ParseControlLine();
		if (controlLine->success)
		{
			return controlLine;
		}
		FreeNode(controlLine);
		BacktrackTo(backtrackPosition);

		AstNode* textLine = ParseTextLine();
		if (textLine->success)
		{
			return textLine;
		}
		FreeNode(textLine);
		BacktrackTo(backtrackPosition);

		// TODO: Add support for non-directives they look like:
		// TODO: #
		// TODO: a pound symbol followed by a newline
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseIfSection()
	{
		int backtrackPosition = CurrentToken;
		AstNode* ifGroup = ParseIfGroup();
		if (ifGroup->success)
		{
			// Optional
			AstNode* elifGroups = ParseElifGroups();
			AstNode* elseGroup = ParseElseGroup();
			Consume(TokenType::MACRO_ENDIF);
			return GenerateIfSectionNode(ifGroup, elifGroups, elseGroup);
		}

		FreeNode(ifGroup);
		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseIfGroup()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::MACRO_IF))
		{
			AstNode* constantExpression = ParseConstantExpression();
			if (constantExpression->success)
			{
				// TODO: Consume a new line here?
				// optional
				AstNode* group = ParseGroup();
				return GenerateIfGroupNode(constantExpression, group);
			}
			FreeNode(constantExpression);
		}
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::MACRO_IFDEF))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				// TODO: consume newline here
				// Optional
				AstNode* group = ParseGroup();
				return GenerateIfDefGroupNode(identifier, group);
			}
		}

		if (Match(TokenType::MACRO_IFNDEF))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				// TODO: consume newline here
				// Optional
				AstNode* group = ParseGroup();
				return GenerateIfNDefGroupNode(identifier, group);
			}
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseElifGroups()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParseElifGroup();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextGroup = ParseElifGroups();
			result = GenerateElifGroupsNode(result, nextGroup);
			if (!nextGroup->success)
			{
				break;
			}
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseElifGroup()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::MACRO_ELIF))
		{
			AstNode* constantExpression = ParseConstantExpression();
			if (constantExpression->success)
			{
				// TODO: consume newline
				// Optional
				AstNode* group = ParseGroup();
				return GenerateElifGroupNode(constantExpression, group);
			}
			FreeNode(constantExpression);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseElseGroup()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::MACRO_ELSE))
		{
			// TODO: consume newline
			// Optional
			AstNode* group = ParseGroup();
			return GenerateElseGroupNode(group);
		}

		BacktrackTo(backtrackPosition);
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseControlLine()
	{
		int backtrackPosition = CurrentToken;
		if (Match(TokenType::MACRO_INCLUDE))
		{
			AstNode* ppTokens = ParsePPTokens();
			if (ppTokens->success)
			{
				// TODO: Consume newline
				return GenerateMacroIncludeNode(ppTokens);
			}
			FreeNode(ppTokens);
		}
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::MACRO_DEFINE))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				// TODO: make sure this left parenthisis is not preceded by whitespace
				if (Match(TokenType::LEFT_PAREN))
				{
					AstNode* identifierList = ParseIdentifierList();
					if (identifierList->success)
					{
						Match(TokenType::COMMA);
						if (Match(TokenType::DOT))
						{
							Consume(TokenType::DOT);
							Consume(TokenType::DOT);
						}
					}

					Consume(TokenType::RIGHT_PAREN);
					AstNode* replacementList = ParseReplacementList();
					if (replacementList->success)
					{
						// TODO: Consume newline
						return GenerateMacroDefineFunctionNode(identifier, identifierList, replacementList);
					}
					FreeNode(replacementList);
					FreeNode(identifierList);
				}

				AstNode* replacementList = ParseReplacementList();
				if (replacementList->success)
				{
					// TODO: consume newline
					return GenerateMacroDefineNode(identifier, replacementList);
				}
				FreeNode(replacementList);
			}
		}
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::MACRO_UNDEF))
		{
			if (Peek() == TokenType::IDENTIFIER)
			{
				Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
				// TODO: Consume newline
				return GenerateMacroUndefNode(identifier);
			}
		}
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::MACRO_LINE))
		{
			AstNode* ppTokens = ParsePPTokens();
			if (ppTokens->success)
			{
				// TODO: consume newline
				return GenerateMacroLineNode(ppTokens);
			}
			FreeNode(ppTokens);
		}
		BacktrackTo(backtrackPosition);

		if (Match(TokenType::MACRO_ERROR))
		{
			// Optional
			AstNode* ppTokens = ParsePPTokens();
			// TODO: Consume newline
			return GenerateMacroErrorNode(ppTokens);
		}

		if (Match(TokenType::MACRO_PRAGMA))
		{
			// Optional
			AstNode* ppTokens = ParsePPTokens();
			// TODO: consume newline
			return GenerateMacroPragmaNode(ppTokens);
		}

		// TODO: Consume # symbol followed by newline
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseTextLine()
	{
		// Optional
		AstNode* ppTokens = ParsePPTokens();
		// TODO: Consume newline
		return GenerateTextLineNode(ppTokens);
	}

	static AstNode* ParseNonDirective()
	{
		int backtrackPosition = CurrentToken;
		AstNode* ppTokens = ParsePPTokens();
		if (ppTokens->success)
		{
			// TODO: Consume newline
			return GenerateNonDirectiveNode(ppTokens);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseIdentifierList()
	{
		if (Peek() == TokenType::IDENTIFIER)
		{
			AstNode* result = GenerateIdentifierNode(ConsumeCurrent(TokenType::IDENTIFIER));

			while (Match(TokenType::COMMA))
			{
				result = GenerateIdentifierListNode(result, ParseIdentifierList());
			}

			return result;
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseReplacementList()
	{
		// Optional
		AstNode* ppTokens = ParsePPTokens();
		return GenerateReplacementListNode(ppTokens);
	}

	static AstNode* ParsePPTokens()
	{
		int backtrackPosition = CurrentToken;
		AstNode* result = ParsePreprocessingToken();
		if (!result->success)
		{
			FreeNode(result);
			BacktrackTo(backtrackPosition);
			return GenerateNoSuccessAstNode();
		}

		while (true)
		{
			AstNode* nextPPToken = ParsePPTokens();
			result = GeneratePPTokensNode(result, nextPPToken);
			if (!nextPPToken->success)
			{
				break;
			}
		}

		return result;
	}

	static AstNode* ParseNumberLiteral()
	{
		if (Peek() == TokenType::FLOATING_POINT_LITERAL || Peek() == TokenType::INTEGER_LITERAL)
		{
			return GenerateNumberLiteralNode(ConsumeCurrent(Peek()));
		}

		return GenerateNoSuccessAstNode();
	}

	// Preprocessor Stuff
	static AstNode* ParsePreprocessingToken()
	{
		int backtrackPosition = CurrentToken;
		AstNode* headerName = ParseHeaderName();
		if (headerName->success)
		{
			return headerName;
		}
		FreeNode(headerName);
		BacktrackTo(backtrackPosition);

		if (Peek() == TokenType::IDENTIFIER)
		{
			return GenerateIdentifierNode(ConsumeCurrent(TokenType::IDENTIFIER));
		}

		AstNode* numberLiteral = ParseNumberLiteral();
		if (numberLiteral->success)
		{
			return numberLiteral;
		}
		FreeNode(numberLiteral);
		BacktrackTo(backtrackPosition);

		AstNode* characterLiteral = ParseCharacterLiteral();
		if (characterLiteral->success)
		{
			return characterLiteral;
		}
		FreeNode(characterLiteral);
		BacktrackTo(backtrackPosition);

		AstNode* stringLiteral = ParseStringLiteral();
		if (stringLiteral->success)
		{
			return stringLiteral;
		}
		FreeNode(stringLiteral);
		BacktrackTo(backtrackPosition);

		// TODO: Should I do this...?
		//AstNode* preprocessingOpOrPunc = ParsePreprocessingOpOrPunc();
		//if (preprocessingOpOrPunc->success)
		//{
		//	return preprocessingOpOrPunc;
		//}
		//FreeNode(preprocessingOpOrPunc);
		//BacktrackTo(backtrackPosition);

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseHeaderName()
	{
		if (Peek() == TokenType::LEFT_ANGLE_BRACKET)
		{
			Token identifier = ConsumeCurrent(TokenType::IDENTIFIER);
			Consume(TokenType::RIGHT_ANGLE_BRACKET);
			return GenerateHeaderNameNode(identifier);
		}

		if (Peek() == TokenType::STRING_LITERAL)
		{
			Token stringLiteral = ConsumeCurrent(TokenType::STRING_LITERAL);
			return GenerateHeaderNameStringNode(stringLiteral);
		}

		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseCharacterLiteral()
	{
		if (Peek() == TokenType::CHARACTER_LITERAL)
		{
			return GenerateCharacterLiteralNode(ConsumeCurrent(TokenType::CHARACTER_LITERAL));
		}
		return GenerateNoSuccessAstNode();
	}

	static AstNode* ParseStringLiteral()
	{
		if (Peek() == TokenType::STRING_LITERAL)
		{
			Token stringLiteral = ConsumeCurrent(TokenType::STRING_LITERAL);
			return GenerateStringLiteralNode(stringLiteral);
		}

		return GenerateNoSuccessAstNode();
	}

	// TODO: Should I do these...?
	// static AstNode* ParsePreprocessingOpOrPunc();
	// static AstNode* ParseHCharSequence();
	// static AstNode* ParseHChar();
	// static AstNode* ParseQCharSequence();
	// static AstNode* ParseQChar();
}