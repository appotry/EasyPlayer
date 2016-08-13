//
//  MHFile.m
//  YangziNew
//
//  Created by yao wang on 11-11-2.
//  Copyright (c) 2011年 imohoo. All rights reserved.
//

#import "MHFileTool.h"
//#import "ZipArchive.h"

#define kDSRoot @"Documents/"

@implementation MHFileTool

/*
 * 函数作用: 根据文件名称获取资源文件路径
 * 函数参数: filename  文件名称
 * 函数返回值: 返回资源文件路径
 */
+(NSString *)getResourcesFile:(NSString *)fileName
{
	return [[NSBundle mainBundle] pathForResource:fileName ofType:nil];
}

/*
 * 函数作用: 根据文件名称获取Doc目录中文件路径
 * 函数参数: fileName  文件名称
 * 函数返回值: 返回Doc目录中文件路径
 */
+(NSString *)getLocalFilePath:(NSString *) fileName
{
	return [NSString stringWithFormat:@"%@/%@%@", NSHomeDirectory(),kDSRoot,fileName];
}

/*
 * 函数作用: 根据目录名称创建文件夹目录
 * 函数参数: dir 目录名称
 * 函数返回值: 返回文件夹目录名称
 */
+(NSString *)createDir:(NSString *)dir
{
	NSFileManager *fm = [NSFileManager defaultManager];
	NSString *path=[self getLocalFilePath:dir];
	BOOL bDir = NO;
	[fm fileExistsAtPath:path isDirectory:&bDir];
	if(bDir==YES) 
    {
        return path;
    }
    
	NSError *error;
	[fm createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
    
	return path;
}

/*
 * 函数作用: 根据目录路径创建文件夹目录
 * 函数参数: 文件夹目录绝对路径
 * 函数返回值: N/A
 */
+(void)createDirWithDirPath:(NSString *)path
{
	NSFileManager *fm = [NSFileManager defaultManager];
	
	BOOL bDir = NO;
	[fm fileExistsAtPath:path isDirectory:&bDir];
	if(bDir==YES) return;
	
	NSError *error;
	[fm createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:&error];
	return;
}


/*
 * 函数作用: 根据文件名判断Doc目录中是否存在
 * 函数参数: fileName 文件目录名称
 * 函数返回值: 是否存在 BOOL  YES (存在) NO (不存在)
 */
+(BOOL)isExistsFileInDoc:(NSString *)fileName
{
	NSFileManager *fm = [NSFileManager defaultManager];
	return [fm fileExistsAtPath:[self getLocalFilePath:fileName]];
}


/*
 * 函数作用: 根据文件名判断资源文件中是否存在
 * 函数参数: fileName 文件目录名称
 * 函数返回值: 是否存在 BOOL  YES (存在) NO (不存在)
 */
+(BOOL)isExistsFileInRescource:(NSString *)fileName
{
	NSFileManager *fm = [NSFileManager defaultManager];
	return [fm fileExistsAtPath:[self getResourcesFile:fileName]];
}

/*
 * 函数作用: 根据文件路径获取文件名称
 * 函数参数: filepath  文件的绝对路径
 * 函数返回值: 文件名称 (带文件类型的）
 */
+(NSString *)getFileNameByPath:(NSString *)filepath
{
	NSArray *array=[filepath componentsSeparatedByString:@"/"];
	if (array.count==0) return filepath;
	
	return [array objectAtIndex:array.count-1];
}

/*
 * 函数作用:  根据文件路径删除目录
 * 函数参数:  filepath 文件路径
 * 函数返回值: N/A
 */
+(void)removeDirByPath:(NSString *)filepath
{
	NSFileManager *fm = [NSFileManager defaultManager];
	[fm removeItemAtPath:filepath error:nil];
}

/*
 * 函数作用:  根据文件名称删除目录
 * 函数参数:  fileName 文件目录名称
 * 函数返回值: N/A
 */
+(void)removeDirByName:(NSString *)fileName
{
	NSFileManager *fm = [NSFileManager defaultManager];
	[fm removeItemAtPath:[self getLocalFilePath:fileName] error:nil];
}

//函数作用:  获取文件目录下所有文件名称
+(NSArray *)getAllFileByName:(NSString *)path
{
    NSFileManager *defaultManager = [NSFileManager defaultManager];	
	NSArray *array = [defaultManager contentsOfDirectoryAtPath:path error:nil];
    return array;
}

//创建目录Library/Application Support
+ (void)createApplicationSupportDirectory
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains
    (NSApplicationSupportDirectory, NSUserDomainMask, YES);
    NSString *applicationDirectory =  [paths objectAtIndex:0]; 
    
    NSFileManager *manager = [NSFileManager defaultManager];
    if([manager fileExistsAtPath:applicationDirectory] == NO)
    {
        [manager createDirectoryAtPath:applicationDirectory
           withIntermediateDirectories:YES
                            attributes:nil
                                 error:nil];
    }
}

//获取Library/Application Support目录下的文件
+ (NSString *)getApplicationSupportFile:(NSString *)file
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains
    (NSApplicationSupportDirectory, NSUserDomainMask, YES);
    
    NSString *applicationDirectory =  [paths objectAtIndex:0]; 
    
    return [NSString stringWithFormat:@"%@/%@",applicationDirectory,file];
}

//获取Caches目录下的文件
+(NSString *)getCacheFile:(NSString *)file{
	return [NSString stringWithFormat:@"%@/Library/Caches/%@", NSHomeDirectory(),file];
}

//文件夹或文件的大小
+(float)sizeOfDirectory:(NSString *)dir{
    /*
     NSString* docDir = [NSString stringWithFormat:@"%@/%@", NSHomeDirectory(),dir];
     NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager] enumeratorAtPath:docDir];
     */
    
    NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager] enumeratorAtPath:dir];
    
	NSString *pname;
	int64_t s=0;
	while (pname = [direnum nextObject]){
        //NSLog(@"pname   %@",pname);
		NSDictionary *currentdict=[direnum fileAttributes];
		NSString *filesize=[NSString stringWithFormat:@"%@",[currentdict objectForKey:NSFileSize]];
		NSString *filetype=[currentdict objectForKey:NSFileType];
		
		if([filetype isEqualToString:NSFileTypeDirectory]) continue;
		s=s+[filesize longLongValue];
	}
	
	return s*1.0;
}

/*
//压缩文件夹
+ (void)zipFolderForm:(NSString *)srcPath to:(NSString *)destPath
//- (void)zipAtPath:(NSString *)pathZip
{
    //pathZip = [MHFileTool getLocalFilePath:@"x.zip"];
    
    //获取目录下所有文件的名字
    NSFileManager *defaultManager = [NSFileManager defaultManager];	
	NSArray *arr = [defaultManager contentsOfDirectoryAtPath:srcPath error:nil];
    
    if([arr count] > 0)
    {
            
        NSDirectoryEnumerator *direnum = [[NSFileManager defaultManager] enumeratorAtPath:srcPath];
        
        ZipArchive *zipArc = [[ZipArchive alloc] init];
        [zipArc CreateZipFile2:destPath];
        
        NSString *pname;
        
        while (pname = [direnum nextObject]){
            
            NSDictionary *currentdict=[direnum fileAttributes];
            NSString *filetype=[currentdict objectForKey:NSFileType];
            
            if(![filetype isEqualToString:NSFileTypeDirectory])
            {
                NSString *filePath = [NSString stringWithFormat:@"%@/%@",srcPath,pname];
                //NSLog(@"pname   %@",pname);
                //NSLog(@"%@",filePath);
                [zipArc addFileToZip:filePath newname:pname];
            }   
        }
        
        [zipArc CloseZipFile2];
        [zipArc release];
        
    }
}
 */

//获取资源文件下的图片
+ (UIImage *)imageWithResourcesName:(NSString *)imgName
{
    NSString *path = [[NSBundle mainBundle] pathForResource:imgName ofType:nil];
    
    return [UIImage imageWithContentsOfFile:path];
}

@end
