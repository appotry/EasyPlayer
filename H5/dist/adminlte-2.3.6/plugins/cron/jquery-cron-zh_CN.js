


;(function($) {
    "use strict";
    
    var cronInputs={
        period: '<div class="cron-select-period"><label></label><select class="cron-period-select"></select></div>',
        startTime: '<div class="cron-input cron-start-time"><select class="cron-clock-hour"></select>时<select class="cron-clock-minute"></select>分</div>',
        container: '<div class="cron-input"></div>',
        seconds : {
            tag: 'cron-seconds',
            inputs: ['<p>每<select class="cron-seconds-select"></select>秒</p>']
        },
        minutes: {
            tag: 'cron-minutes',
            inputs: [ '<p>每<select class="cron-minutes-select"></select>分钟</p>' ]
        },
        hourly: {
            tag: 'cron-hourly',
            inputs: [ '<p>每<select class="cron-hourly-select"></select>小时</p>']
        },
        daily: {
            tag: 'cron-daily',
            inputs: [ '<p>每<select class="cron-daily-select"></select>天</p>']
        },
        weekly: {
            tag: 'cron-weekly',
            inputs: [ '<p>每周<input type="checkbox" name="dayOfWeekMon">一<input type="checkbox" name="dayOfWeekTue">二'+ 
                '<input type="checkbox" name="dayOfWeekWed">三<input type="checkbox" name="dayOfWeekThu">四' +
                '<input type="checkbox" name="dayOfWeekFri">五<input type="checkbox" name="dayOfWeekSat">六'+ 
                '<input type="checkbox" name="dayOfWeekSun">日</p>' ]
        },
        monthly: {
            tag: 'cron-monthly',
            inputs: [ '<p>每<select class="cron-monthly-month"></select>月<select class="cron-monthly-day"></select>日</p>']
        },
        yearly: {
            tag: 'cron-yearly',
            inputs: [ '<p>每年<select class="cron-yearly-month"></select>月<select class="cron-yearly-day"></select>日</p>']
        }
    };

    var periodOpts=arrayToOptions(["秒", "分", "时", "天", "周", "月", "年"]);
    var secondOpts=rangeToOptions(1, 60);
    var minuteOpts=rangeToOptions(1, 60);
    var hourOpts=rangeToOptions(1, 24);
    var dayOpts=rangeToOptions(1, 100);
    var secondClockOpts=rangeToOptions(0, 59, true);
    var minuteClockOpts=rangeToOptions(0, 59, true);
    var hourClockOpts=rangeToOptions(0, 23, true);
    var dayInMonthOpts=rangeToOptions(1, 31);
    var monthNumOpts=rangeToOptions(1, 12);
    var nthWeekOpts=arrayToOptions(["第一个", "第二个", "第三个", "第四个"], [1,2,3,4]);
    var dayOfWeekOpts=arrayToOptions(["星期一","星期二","星期三","星期四","星期五","星期六", "星期天"], ["1","2","3","4","5","6", "7"]);
    
    
    // Convert an array of values to options to append to select input
    function arrayToOptions(opts, values) {
        var inputOpts='';
        for (var i = 0; i < opts.length; i++) {
            var value=opts[i];
            if(values!=null) value=values[i];
            inputOpts += "<option value='"+value+"'>" + opts[i] + "</option>\n";
            
        }
        return inputOpts;
    }
    
    // Convert an integer range to options to append to select input
    function rangeToOptions(start, end, isClock) {
        var inputOpts='', label;
        for (var i = start; i <= end; i++) {
            if(isClock && i < 10) label = "0" + i;
            else label = i;
            inputOpts += "<option value='"+i+"'>" + label + "</option>\n";
        }
        return inputOpts;
    }
    
    // Add input elements to UI as defined in cronInputs
    function addInputElements($baseEl, inputObj, onFinish) {
        $(cronInputs.container).addClass(inputObj.tag).appendTo($baseEl);
        $baseEl.children("."+inputObj.tag).append(inputObj.inputs);
        if(typeof onFinish === "function") onFinish();
    }
    
    var eventHandlers={
        periodSelect: function() {
            var period=($(this).val());
            var $selector=$(this).parent();
            $selector.siblings('div.cron-input').hide();
            $selector.siblings().find("select option").removeAttr("selected");
            $selector.siblings().find("select option:first").attr("selected", "selected");
            $selector.siblings('div.cron-start-time').show();
            //$selector.siblings('div.cron-start-time').children("select.cron-clock-hour").val('12');
            switch(period) {
                case '秒':
                    $selector.siblings('div.cron-seconds')
                        .show()
                        .find("select.cron-seconds-select").val('1');
                    $selector.siblings('div.cron-start-time').hide();
                    break;
                case '分':
                    $selector.siblings('div.cron-minutes')
                        .show()
                        .find("select.cron-minutes-select").val('1');
                    $selector.siblings('div.cron-start-time').hide();
                    break;
                case '时':
                    var $hourlyEl=$selector.siblings('div.cron-hourly');
                    $hourlyEl.show();
                    $selector.siblings('div.cron-start-time').hide();
                    break;
                case '天':
                    var $dailyEl=$selector.siblings('div.cron-daily');
                    $dailyEl.show()
                        .find("input[name=dailyType][value=every]").prop('checked', true);
                    break;
                case '周':
                    $selector.siblings('div.cron-weekly')
                        .show()
                        .find("input[type=checkbox]").prop('checked', false);
                    break;
                case '月':
                    var $monthlyEl=$selector.siblings('div.cron-monthly');
                    $monthlyEl.show();
                    break;
                case '年':
                    var $yearlyEl=$selector.siblings('div.cron-yearly');
                    $yearlyEl.show();
                    break;                    
            } 
        }
    };
    
    // Public functions
    $.cronBuilder = function(el, options) {
        var base = this;
        
        // Access to jQuery and DOM versions of element
        base.$el=$(el);
        base.el=el;
        
        // Reverse reference to the DOM object
        base.$el.data('cronBuilder', base);
        
        // Initialization
        base.init = function() {
            base.options = $.extend({},$.cronBuilder.defaultOptions, options);
        
            
            base.$el.append(cronInputs.period);
            base.$el.find("div.cron-select-period label").text(base.options.selectorLabel);
            base.$el.find("select.cron-period-select")
                .append(periodOpts)
                .bind("change", eventHandlers.periodSelect);

            addInputElements(base.$el, cronInputs.seconds, function() {
                base.$el.find("select.cron-seconds-select").append(secondOpts);
            });
            
            addInputElements(base.$el, cronInputs.minutes, function() {
                base.$el.find("select.cron-minutes-select").append(minuteOpts);
            });

            addInputElements(base.$el, cronInputs.hourly, function() {
                base.$el.find("select.cron-hourly-select").append(hourOpts);
            });
            
            addInputElements(base.$el, cronInputs.daily, function() {
                base.$el.find("select.cron-daily-select").append(dayOpts); 
            });
            
            addInputElements(base.$el, cronInputs.weekly);
            
            addInputElements(base.$el, cronInputs.monthly, function() {
                base.$el.find("select.cron-monthly-day").append(dayInMonthOpts); 
                base.$el.find("select.cron-monthly-month").append(monthNumOpts); 
            });

            addInputElements(base.$el, cronInputs.yearly, function() {
                base.$el.find("select.cron-yearly-month").append(monthNumOpts); 
                base.$el.find("select.cron-yearly-day").append(dayInMonthOpts);  
            });
            
            
            base.$el.append(cronInputs.startTime);
            base.$el.find("select.cron-clock-hour").append(hourClockOpts);
            base.$el.find("select.cron-clock-minute").append(minuteClockOpts);
            
            if(typeof base.options.onChange === "function") {
                base.$el.find("select, input").change(function() {
                    base.options.onChange(base.getExpression(), base.getText());
                });
            }

            base.$el.find("select.cron-period-select")
                .triggerHandler("change");
            
        }

        base.getText = function(){
            var period = base.$el.find("select.cron-period-select").val();
            var text = "";
            switch(period){
                case '秒':
                    var $selector=base.$el.find("div.cron-seconds");
                    text += "每";
                    text += $selector.find("select.cron-seconds-select option:selected").text();
                    text += "秒";
                    break;
                case '分':
                    var $selector=base.$el.find("div.cron-minutes");
                    text += "每";
                    text += $selector.find("select.cron-minutes-select option:selected").text();
                    text += "分钟";
                    break;
                case '时':
                    var $selector=base.$el.find("div.cron-hourly");
                    text += "每";
                    text += $selector.find("select.cron-hourly-select option:selected").text();
                    text += "小时";
                    break;
                case '天':
                    var $selector=base.$el.find("div.cron-daily");
                    text += "每";
                    text += $selector.find("select.cron-daily-select option:selected").text();
                    text += "天";
                    text += base.$el.find("select.cron-clock-hour option:selected").text();
                    text += "时";
                    text += base.$el.find("select.cron-clock-minute option:selected").text();
                    text += "分";
                    break;
                case '周':
                    var $selector=base.$el.find("div.cron-weekly");
                    var ndow=[];
                    if($selector.find("input[name=dayOfWeekMon]").is(":checked"))
                        ndow.push("一");
                    if($selector.find("input[name=dayOfWeekTue]").is(":checked"))
                        ndow.push("二");
                    if($selector.find("input[name=dayOfWeekWed]").is(":checked"))
                        ndow.push("三");
                    if($selector.find("input[name=dayOfWeekThu]").is(":checked"))
                        ndow.push("四");
                    if($selector.find("input[name=dayOfWeekFri]").is(":checked"))
                        ndow.push("五");
                    if($selector.find("input[name=dayOfWeekSat]").is(":checked"))
                        ndow.push("六");
                    if($selector.find("input[name=dayOfWeekSun]").is(":checked"))
                        ndow.push("日");
                    if(ndow.length < 7 && ndow.length > 0) {
                        text += "每周";
                        text += ndow.join(",");
                        text += base.$el.find("select.cron-clock-hour option:selected").text();
                        text += "时";
                        text += base.$el.find("select.cron-clock-minute option:selected").text();
                        text += "分";
                    }else {
                        text += "每天";
                        text += base.$el.find("select.cron-clock-hour option:selected").text();
                        text += "时";
                        text += base.$el.find("select.cron-clock-minute option:selected").text();
                        text += "分";
                    }
                    break;
                case '月':
                    var $selector=base.$el.find("div.cron-monthly");
                    text += "每";
                    text += $selector.find("select.cron-monthly-month option:selected").text();
                    text += "月";
                    text += $selector.find("select.cron-monthly-day option:selected").text();
                    text += "日";
                    text += base.$el.find("select.cron-clock-hour option:selected").text();
                    text += "时";
                    text += base.$el.find("select.cron-clock-minute option:selected").text();
                    text += "分";                    
                    break;                    
                case '年':
                    var $selector=base.$el.find("div.cron-yearly");
                    text += "每年";
                    text += $selector.find("select.cron-yearly-month option:selected").text();
                    text += "月";
                    text += $selector.find("select.cron-yearly-day option:selected").text();
                    text += "日";
                    text += base.$el.find("select.cron-clock-hour option:selected").text();
                    text += "时";
                    text += base.$el.find("select.cron-clock-minute option:selected").text();
                    text += "分";                    
                    break;                    
            }
            return text;
        }

        base.setExpression = function(cron) {
            cron = cron || "* * * * * *";
            var crons = cron.split(/\s+/);
            if(crons.length != 6) return;
            if(crons[0] == "*"){
                base.$el.find("select.cron-period-select").val('秒').trigger("change");
                base.$el.find("select.cron-seconds-select").val('1').trigger("change");
                return;
            }
            if(crons[0].split('/').length == 2){
                base.$el.find("select.cron-period-select").val('秒').trigger("change");
                base.$el.find("select.cron-seconds-select").val(crons[0].split('/')[1]).trigger("change");
                return;
            }
            if(crons[1] == "*"){
                base.$el.find("select.cron-period-select").val('分').trigger("change");
                base.$el.find("select.cron-minutes-select").val('1').trigger("change");
                return;
            }
            if(crons[1].split('/').length == 2){
                base.$el.find("select.cron-period-select").val('分').trigger("change");
                base.$el.find("select.cron-minutes-select").val(crons[1].split('/')[1]).trigger("change");
                return;
            }
            if(crons[2] == "*"){
                base.$el.find("select.cron-period-select").val('时').trigger("change");
                base.$el.find("select.cron-hourly-select").val('1').trigger("change");
                return;
            }
            if(crons[2].split('/').length == 2){
                base.$el.find("select.cron-period-select").val('时').trigger("change");
                base.$el.find("select.cron-hourly-select").val(crons[2].split('/')[1]).trigger("change");
                return;
            }
            if(crons[3] == "*" &&　crons[5] == "*"){
                base.$el.find("select.cron-period-select").val('天').trigger("change");
                base.$el.find("select.cron-daily-select").val('1').trigger("change");
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");
                return;
            }
            if(crons[3] == "*" && crons[5] != "*"){
                base.$el.find("select.cron-period-select").val('周').trigger("change");
                base.$el.find(".cron-weekly p :checkbox").prop("checked",false);
                var days = crons[5].split(',');
                for(var i in days){
                    if(days[i] == '0')
                        base.$el.find(".cron-weekly input[name=dayOfWeekSun]").prop("checked",true);
                    if(days[i] == '1')
                        base.$el.find(".cron-weekly input[name=dayOfWeekMon]").prop("checked",true);
                    if(days[i] == '2')
                        base.$el.find(".cron-weekly input[name=dayOfWeekTue]").prop("checked",true);
                    if(days[i] == '3')
                        base.$el.find(".cron-weekly input[name=dayOfWeekWed]").prop("checked",true);
                    if(days[i] == '4')
                        base.$el.find(".cron-weekly input[name=dayOfWeekThu]").prop("checked",true);
                    if(days[i] == '5')
                        base.$el.find(".cron-weekly input[name=dayOfWeekFri]").prop("checked",true);
                    if(days[i] == '6')
                        base.$el.find(".cron-weekly input[name=dayOfWeekSat]").prop("checked",true);
                }
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");
                return;
            }
            if(crons[3].split('/').length == 2){
                base.$el.find("select.cron-period-select").val('天').trigger("change");
                base.$el.find("select.cron-daily-select").val(crons[3].split('/')[1]).trigger("change");
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");
                return;
            }
            if(crons[4] == "*"){
                base.$el.find("select.cron-period-select").val('月').trigger("change");
                base.$el.find("select.cron-monthly-month").val('1').trigger("change");
                base.$el.find("select.cron-monthly-day").val(crons[3]).trigger("change");
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");                
                return;
            }
            if(crons[4].split('/').length == 2){
                base.$el.find("select.cron-period-select").val('月').trigger("change");
                base.$el.find("select.cron-monthly-month").val(crons[4].split('/')[1]).trigger("change");
                base.$el.find("select.cron-monthly-day").val(crons[3]).trigger("change");
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");                
                return;
            }
            if(/\d+/.test(crons[4])){
                base.$el.find("select.cron-period-select").val('年').trigger("change");
                base.$el.find("select.cron-yearly-month").val(crons[4]).trigger("change");
                base.$el.find("select.cron-yearly-day").val(crons[3]).trigger("change");
                base.$el.find("select.cron-clock-hour").val(crons[2]).trigger("change");
                base.$el.find("select.cron-clock-minute").val(crons[1]).trigger("change");                
                return;
            }
        }
        
        base.getExpression = function() {
            //var b = c.data("block");
            var sec = 0; // ignoring seconds by default
            var year = "*"; // every year by default
            var dow = "*";
            var month ="*", dom = "*";
            var min=base.$el.find("select.cron-clock-minute").val();
            var hour=base.$el.find("select.cron-clock-hour").val();
            var period = base.$el.find("select.cron-period-select").val();
            switch (period) {
                case '秒':
                    var $selector=base.$el.find("div.cron-seconds");
                    var nsec=$selector.find("select.cron-seconds-select").val();
                	if(nsec > 1) sec ="*/"+nsec;
                	else sec="*";
                    min="*";
                	hour="*";                
                    break;
                case '分':
                    var $selector=base.$el.find("div.cron-minutes");
                    var nmin=$selector.find("select.cron-minutes-select").val();
                	if(nmin > 1) min ="*/"+nmin;
                	else min="*";
                	hour="*";
                    break;
                    
                case '时':
                    var $selector=base.$el.find("div.cron-hourly");
                    min=0;
                    hour="*";
                    var nhour=$selector.find("select.cron-hourly-select").val();
                    if(nhour > 1) hour ="*/"+nhour;
                    break;
    
                case '天':
                    var $selector=base.$el.find("div.cron-daily");
                    var ndom=$selector.find("select.cron-daily-select").val();
                    if(ndom > 1) dom ="*/"+ndom;
                    break;
    
                case '周':
                    var $selector=base.$el.find("div.cron-weekly");
                    var ndow=[];
                    if($selector.find("input[name=dayOfWeekSun]").is(":checked"))
                        ndow.push("0");
                    if($selector.find("input[name=dayOfWeekMon]").is(":checked"))
                        ndow.push("1");
                    if($selector.find("input[name=dayOfWeekTue]").is(":checked"))
                        ndow.push("2");
                    if($selector.find("input[name=dayOfWeekWed]").is(":checked"))
                        ndow.push("3");
                    if($selector.find("input[name=dayOfWeekThu]").is(":checked"))
                        ndow.push("4");
                    if($selector.find("input[name=dayOfWeekFri]").is(":checked"))
                        ndow.push("5");
                    if($selector.find("input[name=dayOfWeekSat]").is(":checked"))
                        ndow.push("6");
                    dow="*";
                    dom="*";
                    if(ndow.length < 7 && ndow.length > 0) dow=ndow.join(",");
                    break;
    
                case '月':
                    var $selector=base.$el.find("div.cron-monthly");
                    var nmonth;
                    month="*";
                    nmonth=$selector.find("select.cron-monthly-month").val();
                    dom=$selector.find("select.cron-monthly-day").val();
                    dow="*";
                    if(nmonth > 1) month ="*/"+nmonth;
                    break;
    
                case '年':
                    var $selector=base.$el.find("div.cron-yearly");
                    dom=$selector.find("select.cron-yearly-day").val();
                    month=$selector.find("select.cron-yearly-month").val();
                    dow="*";
                    break;
    
                default:
                    break;
            }
            return [sec, min, hour, dom, month, dow].join(" ");            
        };
        
        base.init();
        base.setExpression(base.options["cron"]||"* * * * * ?");
    };
    
    // Plugin default options
    $.cronBuilder.defaultOptions = {
        selectorLabel: "选择周期 "
        
    };    
 
    // Plugin definition 
    $.fn.cronBuilder = function(options) {
        return this.each(function(){
            (new $.cronBuilder(this, options));
        });
    };
 
}( jQuery ));
