//
//  JudiceTextType.h
//  JudiceTextType
//
//  Created by Mac on 1/21/14.
//  Copyright (c) 2014 www.uniots.com. All rights reserved.
//
typedef NS_ENUM(NSInteger,CompareResult)
{
    Compare_Big,
    Compare_Small,
    Compare_Equal,
};


#import <Foundation/Foundation.h>

@interface JudiceTextType : NSObject



//是否是整数或小数
+(BOOL)JudiceCheckFloatValue:(NSString *)string;
//是否是指定不大于定长的的字符
+(BOOL)judiceIsString:(NSString *)string haveLength:(NSInteger)length;

//指定不大于的存数字
+(BOOL)judiceIsOnlyNum:(NSString *)string haveLength:(NSInteger)length;
//a-z A-Z 0-9 zigbee地址是否合法
+(BOOL)judiceIsazAndAZAnd09:(NSString *)string;


//版本号比较,比较两个版本号的版本大小,如比较2.01.01 和 2.01.03
+(CompareResult)compareVersion:(NSString *)aVersion andOtherVersion:(NSString *)bVersion;
// 手机号
+ (BOOL)checkTel:(NSString *)str;

//身份证是否合法
+ (BOOL)chk18PaperId:(NSString *)sPaperId;
//姓名是否合法,只能包含汉字,字母,数字,长度2-10
+(BOOL)checkNameType:(NSString *)name;

//判断邮箱
+ (BOOL) validateEmail:(NSString *)email;
//判断是否含有非法字符 yes 有  no没有
+ (BOOL)JudgeTheillegalCharacter:(NSString *)content;
//过滤出数字返回字符串
+ (NSString *)scanStr:(NSString *)str;

//过滤出数字和.
+ (NSString *)scanStrAndPoint:(NSString *)str;

+(NSDictionary *)handleParametersDic:(NSDictionary *)dic;
/*
 数组或字典转json字符串
 */
+ (NSString *)jsonStringWithJsonObject:(id)obj;
/*
 json字符串转字典
 */
+ (NSDictionary *)dictionaryWithjsonStringFromat:(NSString *)jsonString;
//根据系统语言来确认返回值，判断参数的标准
+ (NSString *)basedOnTheSystemLanguageToConfirm;
@end
