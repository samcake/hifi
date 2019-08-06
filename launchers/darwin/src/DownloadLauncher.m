#import "DownloadLauncher.h"
#import "Launcher.h"


@implementation DownloadLauncher

- (void) downloadLauncher:(NSString*)launcherUrl {
    NSURLRequest* request = [NSURLRequest requestWithURL:[NSURL URLWithString:launcherUrl]
                                             cachePolicy:NSURLRequestUseProtocolCachePolicy
                                         timeoutInterval:60.0];

    NSURLSessionConfiguration *defaultConfigObject = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession *defaultSession = [NSURLSession sessionWithConfiguration: defaultConfigObject delegate: self delegateQueue: [NSOperationQueue mainQueue]];
    NSURLSessionDownloadTask *downloadTask = [defaultSession downloadTaskWithRequest:request];
    [downloadTask resume];
}

-(void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didWriteData:(int64_t)bytesWritten totalBytesWritten:(int64_t)totalBytesWritten totalBytesExpectedToWrite:(int64_t)totalBytesExpectedToWrite {
    CGFloat prog = (float)totalBytesWritten/totalBytesExpectedToWrite;
    NSLog(@"Launcher downloaded %f", (100.0*prog));

}

-(void)URLSession:(NSURLSession*)session downloadTask:(NSURLSessionDownloadTask*)downloadTask didFinishDownloadingToURL:(NSURL*)location {
    NSLog(@"Did finish downloading to url");
     NSError* error = nil;
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSString* destinationFileName = downloadTask.originalRequest.URL.lastPathComponent;
    NSString* finalFilePath = [[[Launcher sharedLauncher] getDownloadPathForContentAndScripts] stringByAppendingPathComponent:destinationFileName];
    NSURL *destinationURL = [NSURL URLWithString: [finalFilePath stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLFragmentAllowedCharacterSet]] relativeToURL: [NSURL URLWithString:@"file://"]];
    NSLog(@"desintation %@", destinationURL);
    if([fileManager fileExistsAtPath:[destinationURL path]]) {
        [fileManager removeItemAtURL:destinationURL error:nil];
    }

    NSLog(@"location: %@", location.path);
    NSLog(@"destination: %@", destinationURL);
    BOOL success = [fileManager moveItemAtURL:location toURL:destinationURL error:&error];


    NSLog(success ? @"TRUE" : @"FALSE");
    Launcher* sharedLauncher = [Launcher sharedLauncher];

    if (error) {
        NSLog(@"Download Launcher: failed to move file to destintation -> error: %@", error);
        [sharedLauncher displayErrorPage];
        return;
    }
    NSLog(@"extracting Launcher file");
    BOOL extractionSuccessful = [sharedLauncher extractZipFileAtDestination:[sharedLauncher getDownloadPathForContentAndScripts] :[[sharedLauncher getDownloadPathForContentAndScripts] stringByAppendingString:destinationFileName]];

    if (!extractionSuccessful) {
        [sharedLauncher displayErrorPage];
        return;
    }
    NSLog(@"finished extracting Launcher file");


    [[Launcher sharedLauncher] runAutoupdater];
}

- (void)URLSession:(NSURLSession*)session task:(NSURLSessionTask*)task didCompleteWithError:(NSError*)error {
    NSLog(@"completed; error: %@", error);
    if (error) {
        [[Launcher sharedLauncher] displayErrorPage];
    }
}
@end
