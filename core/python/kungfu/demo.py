import sys
import kungfu.yijinjing.time as kft
from kungfu.wingchun.constants import *
 
source = "ctp"
account = "123456"
tickers = ["rb1911","rb1922"]
volume = 2
exchange = Exchange.SHFE
 
# （系统回调函数）启动前回调函数，添加交易账户，订阅行情，策略初始化计算等
def pre_start(context):
    context.add_account(source, account, 100000.0)
    context.subscribe(source, tickers, exchange)
    context.count = 0
     
# （系统回调函数）启动后回调函数，策略连接上行情交易柜台后调用，本函数回调后，策略可以执行添加时间回调、获取策略持仓、报单等操作
def post_start(context):
    context.log.info("[log] {}".format("post_start"))
    context.add_timer(context.now(),2*1000000000, log_pos)
 
# （系统回调函数）换天的时候回调的函数
def on_trading_day(context, date_time):
    context.log.info("[on_trading_day] {}".format(date_time))
         
# （系统回调函数）在收到行情以后进行下单，在下单成功后，利用时间回调函数，在1s以后进行撤单操作
def on_quote(context, quote):
    #context.log.info("[on_quote] {}".format(quote))
    if quote.instrument_id in tickers:
        if (context.count%100) == 0:
            order_id = context.insert_order(quote.instrument_id, exchange, account, quote.last_price + 0.2, volume, PriceType.Limit, Side.Buy, Offset.Open)
            #股票报单offset 直接填Offset.Open
            if order_id > 0:
                context.log.info("[order] (rid){} (ticker){}".format(order_id, quote.instrument_id))
                context.add_timer(context.now(), 1*1000000000, lambda ctx, event: cancel_order(ctx, order_id))
            context.count += 1
 
# （系统回调函数）订单信息的更新会自动触发调用，并打印相关的订单信息
def on_order(context, order):
    context.log.info("[on_order] {}".format(order))
 
# （系统回调函数）策略订单成交信息的更新会自动触发调用，并打印相关的订单成交信息  
def on_trade(context, trade):
    context.log.info("[on_trade] {}".format(trade))
     
# （系统回调函数）退出前方法
def pre_stop(context):
    context.log.info("strategy will stop")
   
# （系统回调函数）退出前方法
def post_stop(context):
    context.log.info("process will stop")
 
     
     
# （自定义函数）获取策略的可用资金、持仓列表以及标的持仓明细
def log_pos(context, event):
    context.log.info("[avail]{}".format(context.ledger.avail))
    context.log.info("[pos_list]{}".format(context.ledger.positions))
    pos = context.ledger.get_position(context.ledger.positions[0], exchange, Direction.Long)
    #股票策略持仓获取 pos = context.ledger.get_position(context.ledger.positions[0], exchange)
    if pos != None:
        context.log.info("[pos]: {}".format(pos))
 
# （自定义函数）撤单函数
def cancel_order(context, order_id):
    action_id =  context.cancel_order(order_id)
    if action_id > 0:
        context.log.info("[cancel order] (action_id){} (rid){} ".format(action_id, order_id))

print("demo")     