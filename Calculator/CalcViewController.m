//
//  CalcViewController.m
//  Calculator
//
//  Created by drops on 2017/5/6.
//  Copyright © 2017年 drops. All rights reserved.
//

#import "CalcViewController.h"
#import "exp_parser.h"

@interface CalcViewController ()
@property (weak, nonatomic) IBOutlet UILabel *resultLabel;
@property (nonatomic, strong) NSString *expression;
@property (nonatomic, strong) NSString *strResult;
@property (nonatomic, assign) BOOL calculated;

@end

@implementation CalcViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.expression = @"";
    // Do any additional setup after loading the view from its nib.
}
- (IBAction)clearInput:(id)sender {
    self.expression = @"";
    self.resultLabel.text = @"0";
}

- (IBAction)keyCalcExpression:(id)sender {
    UIButton *button = (UIButton *)sender;
    NSString *title  = button.titleLabel.text;
    if ([self p_invalidEXP:title]) {
        return;
    }
         
    if (self.calculated) {
        if (![self p_isOperator:title]) {
            self.expression = @"";
        }
        self.calculated = NO;
    }
    
    if ([title isEqualToString:@"+/_"]) {
        if (self.expression.length == 0) {
            self.expression = [@"-1*" stringByAppendingString:self.expression];
        } else {
            self.expression = [self.expression stringByAppendingString:@"*(-1)"];
        }
        
    } else {
         self.expression = [self.expression stringByAppendingString:title];
    }
   
    self.resultLabel.text = self.expression;
}


//"="
- (IBAction)calculate:(id)sender {
    NSString *title  = ((UIButton *)sender).titleLabel.text;
    if ([title isEqualToString:@"%"]) {
        self.expression = [self.expression stringByAppendingString:@"/100.0"];
    }
    
    char *exp = (char *)[self.expression UTF8String];
    char result[1024]= {0};
    TYPE res_type;
    int res_size = sizeof(result);
    int ret = exp_parser(exp, result, &res_type, res_size);
    printf("ret:%d, result:'%s' type:%d stype:%s\n\n", ret, result, res_type, exp_get_stype(res_type));
    if (ret) {
        self.strResult = @"错误";
    } else {
        self.strResult = [NSString stringWithCString:exp_trim_zero(result) encoding:NSUTF8StringEncoding];
    }
    
    self.expression = self.strResult;
    self.resultLabel.text = self.strResult;
    self.calculated = YES;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark - private methods
- (BOOL)p_isOperator:(NSString *)str {
    return [str isEqualToString:@"/"] || [str isEqualToString:@"*"] || [str isEqualToString:@"-"] || [str isEqualToString:@"+"];
}

- (BOOL)p_invalidEXP:(NSString *)inputString {
    NSString *lastOperator = [NSString stringWithFormat:@"%c", [self.expression characterAtIndex:self.expression.length - 1]];
    if ([self p_isOperator:lastOperator] && [self p_isOperator:inputString]) {
        return YES;
    }
    
    return NO;
}

@end
