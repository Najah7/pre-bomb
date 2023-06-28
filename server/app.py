from flask import Flask, jsonify

from linebot import LineBotApi
from linebot.exceptions import LineBotApiError
from linebot.models import (
    TextMessage, StickerMessage
)

import os
import dotenv
dotenv.load_dotenv()

ACCESS_TOKEN = os.getenv('ACCESS_TOKEN')
USER_ID = os.getenv('USER_ID')

# NOTE: 👇 I own you one sticker
STICKER_PACKAGE_ID = "8522"
STICKER_ID = "16581268"

ROLL_PAPER = "🧻"

MESSAGE_DATA = [
    TextMessage(text=f"トイレットペーパーがなくなりました{ROLL_PAPER}"),
    StickerMessage(package_id=STICKER_PACKAGE_ID, sticker_id=STICKER_ID),
    StickerMessage(package_id=STICKER_PACKAGE_ID, sticker_id=STICKER_ID),
    StickerMessage(package_id=STICKER_PACKAGE_ID, sticker_id=STICKER_ID),
    StickerMessage(package_id=STICKER_PACKAGE_ID, sticker_id=STICKER_ID),
]

app = Flask(__name__)



@app.route('/')
def helth_check():
    return "App is running!"

@app.route('/send')
def send_message():
    try:
        # TODO: LINEで通知（スタ爆）を送る処理を書く
        line_bot_api = LineBotApi(ACCESS_TOKEN)
        line_bot_api.push_message(USER_ID, MESSAGE_DATA)
        return jsonify(
            {"message": "success"},
        )
    except LineBotApiError  as e:
        return jsonify({
            "message": "failed",
            "error": e
            }
        )
    
if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
    
    
