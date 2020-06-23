
//
//  obfuscator.cpp
//  obfuscator
//
//  Created by 冯立海/335418265@qq.com on 2020/2/1.
//  Copyright © 2020 fenglh. All rights reserved.
//

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include <iostream>

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

static cl::OptionCategory ObfOptionCategory("ObfOptionCategory");



// 匹配回调
class MatchCallbackHandler : public  MatchFinder::MatchCallback {
public:
    //构造函数
    MatchCallbackHandler(Rewriter &aRewriter, CompilerInstance *aCompilerInstance):rewriter(aRewriter), compilerInstance(aCompilerInstance)  {}
    virtual void run(const MatchFinder::MatchResult &Result) {
        
        const ObjCInterfaceDecl *interfaceDecl          = Result.Nodes.getNodeAs<ObjCInterfaceDecl>("objcInterfaceDecl");
        const ObjCImplementationDecl *implDecl          = Result.Nodes.getNodeAs<ObjCImplementationDecl>("objcImplementationDecl");
        const ObjCCategoryDecl *categoryDecl            = Result.Nodes.getNodeAs<ObjCCategoryDecl>("objcCategoryDecl");
        const ObjCCategoryImplDecl *categoryImplDecl    = Result.Nodes.getNodeAs<ObjCCategoryImplDecl>("objcCategoryImplDecl");
        const VarDecl *varDecl                          = Result.Nodes.getNodeAs<VarDecl>("varDecl");
        const ObjCIvarDecl *objcIvarDecl                = Result.Nodes.getNodeAs<ObjCIvarDecl>("objcIvarDecl");
        const ObjCPropertyDecl *objcPropertyDecl        = Result.Nodes.getNodeAs<ObjCPropertyDecl>("objcPropertyDecl");
        const ObjCMethodDecl *methodDecl                = Result.Nodes.getNodeAs<ObjCMethodDecl>("objcMethodDecl");
        const ObjCMessageExpr *messageExpr              = Result.Nodes.getNodeAs<ObjCMessageExpr>("objcMessageExpr");
        const ExplicitCastExpr *explicitCastExpr        = Result.Nodes.getNodeAs<ExplicitCastExpr>("explicitCastExpr");
        const clang::StringLiteral *stringLiteral       = Result.Nodes.getNodeAs<clang::StringLiteral>("stringLiteral");
        const TypedefDecl *typedefDecl                  = Result.Nodes.getNodeAs<TypedefDecl>("typedefDecl");
        
        

        if (isUserSourceDecl(interfaceDecl)) handleInterfaceDecl(interfaceDecl);
        if (isUserSourceDecl(implDecl)) handleImplementationDecl(implDecl);
        if (isUserSourceDecl(categoryDecl)) handleCategoryDecl(categoryDecl);
        if (isUserSourceDecl(categoryImplDecl)) handleCategoryImplDecl(categoryImplDecl);
        if (isUserSourceDecl(explicitCastExpr)) handleExplicitCastExpr(explicitCastExpr);
        if (isUserSourceDecl(varDecl)) handleVarDecl(varDecl);
        if (isUserSourceDecl(objcIvarDecl)) handleObjcIVarDecl(objcIvarDecl);
        if (isUserSourceDecl(objcPropertyDecl)) handleObjcPropertyDecl(objcPropertyDecl);
        if (isUserSourceDecl(methodDecl)) handleMethodDecl(methodDecl);
        if (isUserSourceDecl(messageExpr)) handleMessageExpr(messageExpr);
        if (stringLiteral) handleStringLiteral(stringLiteral);
        if (isUserSourceDecl(typedefDecl)) handleTypedefDecl(typedefDecl);

    }
    
    void ReplaceText(SourceLocation Start, unsigned OrigLength,StringRef NewStr){
        if (compilerInstance->getSourceManager().isMacroBodyExpansion(Start)) {
            Start = compilerInstance->getSourceManager().getSpellingLoc(Start);
//            unsigned offset = 0;
//            const char *buffer = compilerInstance->getSourceManager().getCharacterData(Start);
//            size_t size = strlen(buffer);
//            while (offset +1 < size  && *(buffer + offset)=='\\' && *(buffer + offset + 1)=='\n') {
//                   offset += 2;
//            }
//            Start = Start.getLocWithOffset(offset);
        }
        rewriter.ReplaceText(Start, OrigLength, NewStr);
    }
    
    //需要混淆的类名
    bool isNeedObfuscateClassName(string name) {
        return name == "DemoViewController";
    }
    
    string getNewClassName(string oldName){
        return "NewViewController";
    }
    
    //处理类声明
    void handleInterfaceDecl(const ObjCInterfaceDecl *interfaceDecl){
        string oldClassName = interfaceDecl->getNameAsString();
        if (isNeedObfuscateClassName(oldClassName)) {
            string newClassName = getNewClassName(oldClassName);
            SourceLocation loc = interfaceDecl->getLocation();
            ReplaceText(loc , oldClassName.length(), getNewClassName(oldClassName));
            cout << "类声明：" << oldClassName << "替换为：" << newClassName<< endl;
        }
    }
    //处理类定义
    void handleImplementationDecl(const ObjCImplementationDecl *objcImplementationDecl){
        string oldClassName = objcImplementationDecl->getNameAsString();
        if (isNeedObfuscateClassName(oldClassName)) {
            string newClassName = getNewClassName(oldClassName);
            SourceLocation loc = objcImplementationDecl->getLocation();
            ReplaceText(loc , oldClassName.length(), getNewClassName(oldClassName));
            cout << fileNameOfNode(objcImplementationDecl)<< "：类定义：" << oldClassName << "替换为：" << newClassName << endl;
        }

    }
    //处理分类声明
    void handleCategoryDecl(const ObjCCategoryDecl *objcCategoryDecl){
        string oldClassName = objcCategoryDecl->getClassInterface()->getNameAsString();
        
        if (isNeedObfuscateClassName(oldClassName)) {
            string newClassName = getNewClassName(oldClassName);
            SourceLocation loc = objcCategoryDecl->getLocation();
            ReplaceText(loc , oldClassName.length(), getNewClassName(oldClassName));
            cout << fileNameOfNode(objcCategoryDecl)<< "：分类声明：" << oldClassName  << "替换为：" << newClassName << endl;
        }
    }
    //处理分类定义
    void handleCategoryImplDecl(const ObjCCategoryImplDecl *objcCategoryImplDecl){
        string oldClassName = objcCategoryImplDecl->getNameAsString();
        if (isNeedObfuscateClassName(oldClassName)) {
            string newClassName = getNewClassName(oldClassName);
            SourceLocation loc = objcCategoryImplDecl->getLocation();
            ReplaceText(loc , oldClassName.length(), getNewClassName(oldClassName));
            cout << fileNameOfNode(objcCategoryImplDecl)<< "：分类定义："  << oldClassName  << "替换为：" << newClassName<< endl;
        }

    }

    
    //处理消息表达式
    void handleMessageExpr(const ObjCMessageExpr *messageExpr){
        if (messageExpr->isClassMessage()) { //是类方法
            const ObjCInterfaceDecl *objcInterfaceDecl = messageExpr->getReceiverInterface();
            if (isUserSourceDecl(objcInterfaceDecl)) {
                string oldClassName = objcInterfaceDecl->getNameAsString();
                if (isNeedObfuscateClassName(oldClassName)) {
                    string newClassName = getNewClassName(oldClassName);
                    SourceLocation loc = messageExpr->getClassReceiverTypeInfo()->getTypeLoc().getBeginLoc();
                    ReplaceText(loc, oldClassName.length(), newClassName);
                    cout << "消息表达式：" << "[" << oldClassName << " " << messageExpr->getSelector().getAsString() << "]" "替换为：" << "[" << newClassName << " " << messageExpr->getSelector().getAsString() << "]" << endl;
                }
            }
            
        }
    }
    

   //处理显式转换
    void handleExplicitCastExpr(const ExplicitCastExpr *explicitCastExpr){

//        explicitCastExpr->getd
        QualType qualType = explicitCastExpr->getTypeAsWritten();
        SourceLocation typeBeginLoc = explicitCastExpr->getTypeInfoAsWritten()->getTypeLoc().getBeginLoc();
        SourceLocation typeEndLoc = explicitCastExpr->getTypeInfoAsWritten()->getTypeLoc().getEndLoc();
        //宏替换后，是否会处理多余的空格?
        bool success = recursiveHandleQualType(typeBeginLoc,typeEndLoc,qualType);
        
        
        if (success) {
            string rewriteString = rewriter.getRewrittenText(SourceRange(explicitCastExpr->getBeginLoc(), explicitCastExpr->getExprLoc()));
            cout << "显式转换："<< qualType.getAsString() << "替换为：" << rewriteString <<  endl;
        }

        
    }
    
    //处理字符串
    void handleStringLiteral(const clang::StringLiteral *stringLiteral){
        clang::StringRef content  = stringLiteral->getString();
        if (isNeedObfuscateClassName(content)) {
            SourceLocation loc = stringLiteral->getBeginLoc();//loc的位置在
            string newClassName = getNewClassName(content);
            ReplaceText(loc.getLocWithOffset(-1), content.str().length()+2, newClassName);
            cout << "字符串：" << content.str() << " 替换为：" << newClassName << endl;
        } 
    }
    
    //处理typeDef
    void handleTypedefDecl(const TypedefDecl *typedefDecl){
        //underlyingType
        QualType type = typedefDecl->getUnderlyingType();
        recursiveHandleQualType(typedefDecl->getBeginLoc(), typedefDecl->getEndLoc(), type);
    }
    
    //处理变量声明或者定义
    void handleVarDecl(const VarDecl *varDecl){
        
        ////隐式实现的，并非在显示的写源码中则不作处理。例如：编译器会实现 property 的实例变量
        if (varDecl->isImplicit()) {
            return;
        }
        QualType qualType = varDecl->getType();
        //隐式实现的Decl ，则varDecl->getTypeSourceInfo() 为NULL
        if (qualType.isNull()) {
            return;
        }
        
        TypeSourceInfo *typeSourceInfo = varDecl->getTypeSourceInfo();
        //即使varDecl->isImplicit() == false ，typeSourceInfo 也会为NULL，所以下面再次进行NULL判断
        if (!typeSourceInfo) {
            return;
        }
        SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
        SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
        bool success =  recursiveHandleQualType(typeBeginLoc,typeEndLoc,qualType);
        if(success){
            SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(varDecl->getBeginLoc());
            SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(varDecl->getEndLoc());
            string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
            cout << "变量声明："  << qualType.getAsString()  << " " << varDecl->getNameAsString() << "替换为：" << rewriteString << endl;
        }

    }
    //处理实例变量
    void handleObjcIVarDecl(const ObjCIvarDecl *objcIvarDecl){
        ////隐式实现的，并非在显示的写源码中则不作处理。例如：编译器会实现 property 的实例变量
        if (objcIvarDecl->isImplicit()) {
            return;
        }
        QualType qualType = objcIvarDecl->getType();
        //隐式实现的Decl ，则varDecl->getTypeSourceInfo() 为NULL
        if (qualType.isNull()) {
            return;
        }
        TypeSourceInfo *typeSourceInfo = objcIvarDecl->getTypeSourceInfo();
        //即使varDecl->isImplicit() == false ，typeSourceInfo 也会为NULL，所以下面再次进行NULL判断
        if (!typeSourceInfo) {
            return;
        }
        SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
        SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
        bool success =  recursiveHandleQualType(typeBeginLoc,typeEndLoc,qualType);
        if(success){
            SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(objcIvarDecl->getBeginLoc());
            SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(objcIvarDecl->getEndLoc());
            string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
            cout << "变量声明："  << qualType.getAsString()  << " " << objcIvarDecl->getNameAsString() << "替换为：" << rewriteString << endl;
        }

    }
    //处理属性
    void handleObjcPropertyDecl(const ObjCPropertyDecl *objcPropertyDecl){

        QualType qualType = objcPropertyDecl->getType();
        TypeSourceInfo *typeSourceInfo = objcPropertyDecl->getTypeSourceInfo();
        if (!typeSourceInfo) {
            return;
        }
        SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
        SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
        bool success =  recursiveHandleQualType(typeBeginLoc,typeEndLoc,qualType);
        if(success){
            SourceLocation beginLoc = compilerInstance->getSourceManager().getSpellingLoc(objcPropertyDecl->getBeginLoc());
            SourceLocation endLoc = compilerInstance->getSourceManager().getSpellingLoc(objcPropertyDecl->getEndLoc());
            string rewriteString = rewriter.getRewrittenText(SourceRange(beginLoc, endLoc));
            cout << "属性声明："  << qualType.getAsString()  << " " << objcPropertyDecl->getNameAsString() << "替换为：" << rewriteString << endl;
        }
    }
    
    
    //处理方法声明或者定义
    void handleMethodDecl(const ObjCMethodDecl *methodDecl){
        
        //隐式实现的，并非在显示的写源码中则不作处理。例如：编译器会实现 property 的getter 和 setter 方法
        if (methodDecl->isImplicit()) {
            return;
        }
        
        //1. 获取返回类型
        QualType qualType = methodDecl->getReturnType();
        if (qualType.isNull()) {
            return;
        }
        TypeSourceInfo *typeSourceInfo = methodDecl->getReturnTypeSourceInfo();
        if (!typeSourceInfo) {
            return;
        }
        //2. 获取type的开始和结束位置，注：当methodDecl是隐式实现的，methodDecl->getReturnTypeSourceInfo()->getTypeLoc() 为NULL
        SourceLocation typeBeginLoc = typeSourceInfo->getTypeLoc().getBeginLoc();
        SourceLocation typeEndLoc = typeSourceInfo->getTypeLoc().getEndLoc();
        //3. 递归处理返回类型
        bool handleReturnTypeSuccess = recursiveHandleQualType(typeBeginLoc, typeEndLoc, qualType);
        //4. 获取参数列表
        ArrayRef<ParmVarDecl*> params = methodDecl->parameters();
        bool handleParamersTypeSuccess = false;
        //5. 遍历参数列表
        for(ArrayRef< ParmVarDecl * >::iterator i = params.begin(), e = params.end(); i != e; i++){
            ParmVarDecl *p = *i;
            //6. 获取参数的类型
            QualType type = p->getType();
            //7. 递归处理参数类型
            handleParamersTypeSuccess = handleParamersTypeSuccess || recursiveHandleQualType(p->getBeginLoc(), p->getEndLoc(), type);
        }
        
        if (handleReturnTypeSuccess || handleParamersTypeSuccess) {
            //-1 是去掉左花括号"{"
            string rewriteString = rewriter.getRewrittenText(SourceRange(methodDecl->getBeginLoc(), methodDecl->getDeclaratorEndLoc().getLocWithOffset(-1)));
            cout << "方法声明/定义：" << getMethodDeclStringOfMethoddecl(methodDecl) << "替换为："<< rewriteString<< endl;
        }
        

        
    }
    
    //递归处理, ture 表示处理成功，false 表示失败，或者无需处理
    bool recursiveHandleQualType(SourceLocation start , SourceLocation end,  QualType type) {
        
       if (start.isInvalid() || end.isInvalid() ) {
            return  false;
       }
            
        bool success = false;
        SourceLocation slideLoc = start;
        if (isa<ObjCObjectPointerType>(type)) { //ObjCObjectPointerType类型
            const ObjCObjectPointerType *pointerType = type->getAs<ObjCObjectPointerType>(); //指针类型
            const ObjCInterfaceDecl *IDecl = pointerType->getInterfaceDecl();
            if (isUserSourceDecl(IDecl) && isNeedObfuscateClassName(IDecl->getNameAsString())) { //是用户源码,并且类名是
                string oldClassName = IDecl->getNameAsString();
                string newClassName = getNewClassName(oldClassName);
                //获取类型声明的字符数据的开始指针
                const char* startBuffer = compilerInstance->getSourceManager().getCharacterData(slideLoc);
                //获取类型声明的字符数据的结束指针
                const char* endBuffer = compilerInstance->getSourceManager().getCharacterData(end);
                //两个指针的偏移量
                int offset = endBuffer - startBuffer;
                //获取雷声声明的字符窜 例如:NSSArray <DemoViewController *>
                string originTypeDefineStr(startBuffer, offset);
                
                //查找该字符串中包含oldClassName 字符串的位置
                int index = originTypeDefineStr.find(oldClassName, 0);
                
                slideLoc =slideLoc .getLocWithOffset(index);
                ReplaceText(slideLoc, oldClassName.length(), newClassName);
                slideLoc = slideLoc.getLocWithOffset(index+oldClassName.length());
                success = true;
            }
            
            if (pointerType->isSpecialized()) { //有类型参数
                const ArrayRef< QualType > params = pointerType->getTypeArgs();
                unsigned index = 0;
                 for(ArrayRef< QualType >::iterator i = params.begin(), e = params.end(); i != e; i++,index++){
                     QualType t = *i;
                    success = success || recursiveHandleQualType(slideLoc, end, t);
                 }
            }
        }else if(isa<AttributedType>(type)){
            const AttributedType *attributedType = type->getAs<AttributedType>(); //指针类型
            success = recursiveHandleQualType(slideLoc, end, attributedType->getModifiedType());
        }
        
        return success;
    }
    
    string getMethodDeclStringOfMethoddecl(const ObjCMethodDecl *methodDecl){
        string methodDeclStr;

        methodDeclStr += (methodDecl->isInstanceMethod()?"-":"+");
        methodDeclStr += "(";
        methodDeclStr += methodDecl->getReturnType().getAsString();
        methodDeclStr += ")";
        int numSelectorLocs = methodDecl->getNumSelectorLocs();//段个数
        int numParams = methodDecl->getSelector().getNumArgs();//参数个数
        for (int i = 0; i < numSelectorLocs; i++) {
            methodDeclStr += methodDecl->getSelector().getNameForSlot(i).str();
            if (i < numParams) {
                methodDeclStr += ":";
                const ParmVarDecl *paramVarDecl = methodDecl->getParamDecl(i);
                methodDeclStr += "(";
                methodDeclStr += paramVarDecl->getType().getAsString();
                methodDeclStr += ")";
                methodDeclStr += paramVarDecl->getNameAsString();
            }
            if (i+1 < numSelectorLocs) {
                methodDeclStr += " ";
            }
        }
        
        return methodDeclStr;
    }
    
    
    

    //获取方法对应的类名或者协议名
    string getClassNameOfMethodDecl(const ObjCMethodDecl *methodDecl){
        string className;
        methodDecl->getClassInterface();

        if (isa<ObjCCategoryDecl>(methodDecl->getDeclContext())) { //
            const ObjCCategoryDecl *categoryDecl = dyn_cast_or_null<ObjCCategoryDecl>(methodDecl->getDeclContext());
            const ObjCInterfaceDecl *interfaceDecl = categoryDecl->getClassInterface();
            if (interfaceDecl) className = interfaceDecl->getNameAsString();
        }else if (isa<ObjCCategoryImplDecl>(methodDecl->getDeclContext())) {
            const ObjCCategoryImplDecl *categoryImplDecl = dyn_cast_or_null<ObjCCategoryImplDecl>(methodDecl->getDeclContext());
            const ObjCInterfaceDecl *interfaceDecl = categoryImplDecl->getClassInterface();
            if (interfaceDecl) className = interfaceDecl->getNameAsString();
        }else if (isa<ObjCInterfaceDecl>(methodDecl->getDeclContext())) {
            const ObjCInterfaceDecl *interfaceDecl = dyn_cast_or_null<ObjCInterfaceDecl>(methodDecl->getDeclContext());
            if (interfaceDecl) className = interfaceDecl->getNameAsString();
        }else if (isa<ObjCImplementationDecl>(methodDecl->getDeclContext())) {
            const ObjCImplementationDecl *implementationDecl = dyn_cast_or_null<ObjCImplementationDecl>(methodDecl->getDeclContext());
            if (implementationDecl) className = implementationDecl->getNameAsString();
        }else { //ObjCProtocolDecl
            const ObjCProtocolDecl *protocolDecl = dyn_cast_or_null<ObjCProtocolDecl>(methodDecl->getDeclContext());
            if (protocolDecl) className = protocolDecl->getNameAsString();
        }
        return className;
    }
    
   //判断是否用户源码，过滤掉系统源码
   template <typename Node>
   bool isUserSourceDecl(const Node node) {
         if(!node)return false;
        string filename = sourcePathNode(node);
        if (filename.empty())
            return false;
        //非XCode中的源码都认为是用户源码
        if(filename.find("/Applications/Xcode.app/") == 0)
            return false;
        return true;
    }
    //获取decl所在的文件
    template <typename Node>
    string sourcePathNode(const Node node ) {
        if(!node)return "";
        
        SourceLocation spellingLoc = compilerInstance->getSourceManager().getSpellingLoc(node->getSourceRange().getBegin());
        string filePath = compilerInstance->getSourceManager().getFilename(spellingLoc).str();
        return filePath;
    }
    
     //获取文件名，截取'/'后面的部分
    template <typename Node>
    string fileNameOfNode(const Node node) {
        string filePath = sourcePathNode(node);
        size_t index = filePath.find_last_of("/");
        if (index == StringRef::npos) {
            return "";
        }
        StringRef fileName = filePath.substr(index+1,-1);
        return fileName.str();
    }

    
private:
    Rewriter &rewriter;
    CompilerInstance *compilerInstance;
};

//AST 构造器
class ObfASTConsumer : public ASTConsumer {
public:
    ObfASTConsumer(Rewriter &aRewriter,CompilerInstance *aCI) :handlerMatchCallback(aRewriter,aCI)  {

        //类声明
        matcher.addMatcher(objcInterfaceDecl().bind("objcInterfaceDecl"),   &handlerMatchCallback);
        //类定义
        matcher.addMatcher(objcImplementationDecl().bind("objcImplementationDecl"),   &handlerMatchCallback);
        //分类声明
        matcher.addMatcher(objcCategoryDecl().bind("objcCategoryDecl"),   &handlerMatchCallback);
        //分类定义
        matcher.addMatcher(objcCategoryImplDecl().bind("objcCategoryImplDecl"),   &handlerMatchCallback);
        //方法声明
        matcher.addMatcher(objcMethodDecl().bind("objcMethodDecl"),   &handlerMatchCallback);
        //变量声明或定义
        matcher.addMatcher(varDecl().bind("varDecl"),   &handlerMatchCallback);
        //实例变量
        matcher.addMatcher(objcIvarDecl().bind("objcIvarDecl"),   &handlerMatchCallback);
        //属性声明
        matcher.addMatcher(objcPropertyDecl().bind("objcPropertyDecl"),   &handlerMatchCallback);
        //类消息表达式
        matcher.addMatcher(objcMessageExpr(isClassMessage()).bind("objcMessageExpr"),   &handlerMatchCallback);
        //显式转换表达式
        matcher.addMatcher(explicitCastExpr().bind("explicitCastExpr"),   &handlerMatchCallback);
        //typedef 声明
        matcher.addMatcher(typedefDecl().bind("typedefDecl"),   &handlerMatchCallback);
        //字符串，小端
        matcher.addMatcher(stringLiteral().bind("stringLiteral"),   &handlerMatchCallback);
        
        

    }

    void HandleTranslationUnit(ASTContext &Context) override {
        //运行匹配器
        matcher.matchAST(Context);
    }
    
    private:
    MatchFinder matcher;
    MatchCallbackHandler handlerMatchCallback;

};

//action
class ObfASTFrontendAction : public ASTFrontendAction {
public:
    //创建AST Consumer
    std::unique_ptr<ASTConsumer> CreateASTConsumer(clang::CompilerInstance &CI, StringRef file) override {
        size_t index = file.str().find_last_of("/");
        StringRef fileName = file.str().substr(index+1,-1); //获取文件名，截取'/'后面的部分
        cout << "【混淆】开始处理文件：" << fileName.str()  <<  endl;
        
        rewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
        return std::make_unique<ObfASTConsumer>(rewriter,&CI);
    }
    //源文件操作结束
    void EndSourceFileAction() override {
        
        SourceManager &SM = rewriter.getSourceMgr();
        
        string Filename = SM.getFileEntryForID(SM.getMainFileID())->getName();
        
        std::error_code error_code;
        llvm::raw_fd_ostream outFile(Filename, error_code, llvm::sys::fs::F_None);
        rewriter.getEditBuffer(SM.getMainFileID()).write(outFile);
        
        //文件处理完成
        cout << "【混淆】文件处理完成: " << Filename << endl;
    }
private:
Rewriter rewriter;

};

int main(int argc, const char **argv) {
    CommonOptionsParser OptionsParser(argc, argv, ObfOptionCategory);
    ClangTool Tool(OptionsParser.getCompilations(),OptionsParser.getSourcePathList());
    return Tool.run(newFrontendActionFactory<ObfASTFrontendAction>().get());
}
