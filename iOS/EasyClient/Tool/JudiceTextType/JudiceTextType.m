//
//  JudiceTextType.m
//  JudiceTextType
//
//  Created by Mac on 1/21/14.
//  Copyright (c) 2014 www.uniots.com. All rights reserved.
//

#import "JudiceTextType.h"

@implementation JudiceTextType



+(CompareResult)compareVersion:(NSString *)aVersion andOtherVersion:(NSString *)bVersion
{
    if (aVersion.length != 7 || bVersion.length !=7)
    {
        return Compare_Equal;
    }
    
    if ([aVersion isEqualToString:bVersion])
    {
        return Compare_Equal;
    }
    else
    {   //判断第一层
        NSString *aFirstStr = [aVersion substringToIndex:1];
        NSString *bFirstStr = [bVersion substringToIndex:1];
        
        if ([self compareAString:aFirstStr andBString:bFirstStr] == Compare_Big)
        {
            return Compare_Big;
        }
        else if ([self compareAString:aFirstStr andBString:bFirstStr] == Compare_Small)
        {
            return Compare_Small;
        }
        else
        {   //第一层相等，判断第二层
            NSRange range = {2,2};
            NSString *aSecondStr = [aVersion substringWithRange:range];
            NSString *bSecondStr = [bVersion substringWithRange:range];
            
            if ([self compareAString:aSecondStr andBString:bSecondStr] == Compare_Big)
            {
                return Compare_Big;
            }
            else if ([self compareAString:aSecondStr andBString:bSecondStr] == Compare_Small)
            {
                return Compare_Small;
            }
            else
            {   //第二层相等，判断第三层
                NSRange rang2 = {5,2};
                NSString *aThirdStr = [aVersion substringWithRange:rang2];
                NSString *bThirdStr = [bVersion substringWithRange:rang2];
                
                if ([self compareAString:aThirdStr andBString:bThirdStr] == Compare_Big)
                {
                    return Compare_Big;
                }
                else if ([self compareAString:aThirdStr andBString:bThirdStr] == Compare_Small)
                {
                    return Compare_Small;
                }
                else
                {
                    return Compare_Equal;
                }
            }
        }
    }
}

#pragma mark - 私有方法

+ (CompareResult )compareAString:(NSString *)aString andBString:(NSString *)bString
{
    NSInteger aStrInter = [aString integerValue];
    NSInteger bStrInter = [bString integerValue];
    
    if (aStrInter > bStrInter)
    {
        return Compare_Big;
    }
    else if (aStrInter < bStrInter)
    {
        return Compare_Small;
    }
    else
    {
        return Compare_Equal;
    }
}
//判断邮箱
+ (BOOL) validateEmail:(NSString *)email
{
    NSString *emailRegex = @"^([a-zA-Z0-9_\\-\\.]+)@((\\[[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.)|(([a-zA-Z0-9\\-]+\\.)+))([a-zA-Z]{2,4}|[0-9]{1,3})(\\]?)$";
    NSPredicate *emailTest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", emailRegex];
    if (![emailTest evaluateWithObject:email])
    {
        return NO;
    }
    return YES;
}
#pragma mark  -telNumber
+ (BOOL)checkTel:(NSString *)str

{
    
    NSString *regex = @"^((13[0-9])|(147)|(15[^4,\\D])|(18[0,3-9])|(17[0-9]))\\d{8}$";
    
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regex];
    
    BOOL isMatch = [pred evaluateWithObject:str];
    
    if (!isMatch) {
        return NO;
    }
    
    
    
    return YES;
    
}
+(BOOL)checkNameType:(NSString *)name{
    NSString * all  = @"^[\\u4E00-\\u9FA5\\uF900-\\uFA2D\\w]{2,10}$";
    NSPredicate *pred = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", all];
    
    BOOL isMatch = [pred evaluateWithObject:name];
    
    return isMatch;
}


#pragma mark - ID
/**
 
 * 功能:获取指定范围的字符串
 
 * 参数:字符串的开始小标
 
 * 参数:字符串的结束下标
 
 */

+ (NSString *)getStringWithRange:(NSString *)str Value1:(NSInteger *)value1 Value2:(NSInteger )value2;

{
    return [str substringWithRange:NSMakeRange((long)value1,(long)value2)];
}



/**
 
 * 功能:判断是否在地区码内
 
 * 参数:地区码
 
 */

+ (BOOL)areaCode:(NSString *)code

{
    
    NSMutableDictionary *dic = [[NSMutableDictionary alloc] init];
    
    [dic setObject:@"北京" forKey:@"11"];
    
    [dic setObject:@"天津" forKey:@"12"];
    
    [dic setObject:@"河北" forKey:@"13"];
    
    [dic setObject:@"山西" forKey:@"14"];
    
    [dic setObject:@"内蒙古" forKey:@"15"];
    
    [dic setObject:@"辽宁" forKey:@"21"];
    
    [dic setObject:@"吉林" forKey:@"22"];
    
    [dic setObject:@"黑龙江" forKey:@"23"];
    
    [dic setObject:@"上海" forKey:@"31"];
    
    [dic setObject:@"江苏" forKey:@"32"];
    
    [dic setObject:@"浙江" forKey:@"33"];
    
    [dic setObject:@"安徽" forKey:@"34"];
    
    [dic setObject:@"福建" forKey:@"35"];
    
    [dic setObject:@"江西" forKey:@"36"];
    
    [dic setObject:@"山东" forKey:@"37"];
    
    [dic setObject:@"河南" forKey:@"41"];
    
    [dic setObject:@"湖北" forKey:@"42"];
    
    [dic setObject:@"湖南" forKey:@"43"];
    
    [dic setObject:@"广东" forKey:@"44"];
    
    [dic setObject:@"广西" forKey:@"45"];
    
    [dic setObject:@"海南" forKey:@"46"];
    
    [dic setObject:@"重庆" forKey:@"50"];
    
    [dic setObject:@"四川" forKey:@"51"];
    
    [dic setObject:@"贵州" forKey:@"52"];
    
    [dic setObject:@"云南" forKey:@"53"];
    
    [dic setObject:@"西藏" forKey:@"54"];
    
    [dic setObject:@"陕西" forKey:@"61"];
    
    [dic setObject:@"甘肃" forKey:@"62"];
    
    [dic setObject:@"青海" forKey:@"63"];
    
    [dic setObject:@"宁夏" forKey:@"64"];
    
    [dic setObject:@"新疆" forKey:@"65"];
    
    [dic setObject:@"台湾" forKey:@"71"];
    
    [dic setObject:@"香港" forKey:@"81"];
    
    [dic setObject:@"澳门" forKey:@"82"];
    
    [dic setObject:@"国外" forKey:@"91"];
    
    if ([dic objectForKey:code] == nil) {
        
        return NO;
        
    }
    
    return YES;
    
}



/**
 
 * 功能:验证身份证是否合法
 
 * 参数:输入的身份证号
 
 */

+ (BOOL) chk18PaperId:(NSString *) sPaperId

{
    
    //判断位数
    
    
    if ([sPaperId length] != 15 && [sPaperId length] != 18) {
        return NO;
    }
    NSString *carid = sPaperId;
    
    long lSumQT =0;
    
    //加权因子
    
    int R[] ={7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2 };
    
    //校验码
    
    unsigned char sChecker[11]={'1','0','X', '9', '8', '7', '6', '5', '4', '3', '2'};
    
    
    
    //将15位身份证号转换成18位
    
    NSMutableString *mString = [NSMutableString stringWithString:sPaperId];
    
    if ([sPaperId length] == 15) {
        
        [mString insertString:@"19" atIndex:6];
        
        long p = 0;
        
        const char *pid = [mString UTF8String];
        
        for (int i=0; i<=16; i++)
            
        {
            
            p += (pid[i]-48) * R[i];
            
        }
        
        int o = p%11;
        
        NSString *string_content = [NSString stringWithFormat:@"%c",sChecker[o]];
        
        [mString insertString:string_content atIndex:[mString length]];
        
        carid = mString;
        
    }
    
    //判断地区码
    
    NSString * sProvince = [carid substringToIndex:2];
    
    if (![JudiceTextType areaCode:sProvince]) {
        
        return NO;
        
    }
    
    //判断年月日是否有效
    
    
    
    //年份
    
    int strYear = [[self getStringWithRange:carid Value1:6 Value2:4] intValue];
    
    //月份
    
    int strMonth = [[self getStringWithRange:carid Value1:10 Value2:2] intValue];
    
    //日
    
    int strDay = [[self getStringWithRange:carid Value1:12 Value2:2] intValue];
    
    
    
    NSTimeZone *localZone = [NSTimeZone localTimeZone];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init] ;
    
    [dateFormatter setDateStyle:NSDateFormatterMediumStyle];
    
    [dateFormatter setTimeStyle:NSDateFormatterNoStyle];
    
    [dateFormatter setTimeZone:localZone];
    
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    
    NSDate *date=[dateFormatter dateFromString:[NSString stringWithFormat:@"%d-%d-%d 12:01:01",strYear,strMonth,strDay]];
    
    if (date == nil) {
        
        return NO;
        
    }
    
    const char *PaperId  = [carid UTF8String];
    
    //检验长度
    
    if( 18 != strlen(PaperId)) return -1;
    
    
    
    //校验数字
    
    for (int i=0; i<18; i++)
        
    {
        
        if ( !isdigit(PaperId[i]) && !(('X' == PaperId[i] || 'x' == PaperId[i]) && 17 == i) )
            
        {
            
            return NO;
            
        }
        
    }
    
    //验证最末的校验码
    
    for (int i=0; i<=16; i++)
        
    {
        
        lSumQT += (PaperId[i]-48) * R[i];
        
    }
    
    if (sChecker[lSumQT%11] != PaperId[17] )
        
    {
        
        return NO;
        
    }
    
    return YES;
    
}

//过滤出数字
+ (NSString *)scanStr:(NSString *)str
{
    NSRegularExpression *regular = [NSRegularExpression regularExpressionWithPattern:@"[^0-9]" options:0 error:NULL];
    
    NSString *result = [regular stringByReplacingMatchesInString:str options:0 range:NSMakeRange(0, [str length]) withTemplate:@""];
    NSLog(@"%@", result);
    return result;
}

//过滤出数字和.
+ (NSString *)scanStrAndPoint:(NSString *)str
{
    NSRegularExpression *regular = [NSRegularExpression regularExpressionWithPattern:@"[^0-9.]+" options:0 error:NULL];
    
    NSString *result = [regular stringByReplacingMatchesInString:str options:0 range:NSMakeRange(0, [str length]) withTemplate:@""];
    NSLog(@"%@", result);
    return result;
}

//判断是否含有非法字符 yes 有  no没有
+ (BOOL)JudgeTheillegalCharacter:(NSString *)content{
    //提示 标签不能输入特殊字符
    NSString *str =@"^[A-Za-z0-9\\u4e00-\u9fa5]+$";
    NSPredicate* emailTest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", str];
    if (![emailTest evaluateWithObject:content]) {
        return YES;
    }
    return NO;
}

+ (NSString *)jsonStringWithJsonObject:(id)obj{
    if (obj == nil) {
        return nil;
    }
    NSData * jsonData = [NSJSONSerialization dataWithJSONObject:obj options:NSJSONWritingPrettyPrinted error:nil];
    return [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
}
+ (id)dictionaryWithjsonStringFromat:(NSString *)jsonString{
    if (jsonString == nil) {
        return nil;
    }
    NSData * data = [jsonString dataUsingEncoding:NSUTF8StringEncoding];
    NSDictionary * dic = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
    return dic;
}


//根据系统语言来确认返回值，判断参数的标准
+ (NSString *)basedOnTheSystemLanguageToConfirm
{
    NSArray *languages = [NSLocale preferredLanguages];
    NSString *currentLanguage = [languages objectAtIndex:0];
    NSLog ( @"%@" , currentLanguage);
    // 英语
    if ([currentLanguage isEqualToString:@"zh-Hans"])
    {
        //简体中文
        return @"zh_cn";
        
    }else if ([currentLanguage isEqualToString:@"zh-Hant-US"] ||[currentLanguage isEqualToString:@"zh-TW"]||[currentLanguage isEqualToString:@"zh-HK"])
    {
        //繁体（香港，澳门）
        return @"zh_tw";
    }else{
        //英语
        return @"en_us";
    }
}
@end
