import { useState, useEffect } from "react"
import useWebSocket, { ReadyState } from "react-use-websocket"
import { ToastContainer, toast } from "react-toastify"
import Draggable, {
    DraggableData,
    DraggableEvent
} from "react-draggable"
import { mapCords, generateMessage, debounce } from "./helpers"
import "react-toastify/dist/ReactToastify.css"
import "./App.css"
import useDebounce from "./hooks/useDebounce"

const CONTROL_SIZE = 84
type TPlayer = "Player1" | "Player2" | null

const DEBOUNCE_TIME = 40
let flag = false

function App() {
    const { sendMessage, lastMessage, readyState } = useWebSocket(
        "ws://192.168.4.1:81"
    )
    const [player, setPlayer] = useState<TPlayer>(null)
    const [cords, setCords] = useState({
        x: window.innerWidth / 2 - CONTROL_SIZE / 2,
        y: window.innerHeight / 2 - CONTROL_SIZE / 2
    })

    useEffect(() => {
        console.log(lastMessage)

        if (lastMessage?.data === "Player1") setPlayer("Player1")
        else if (lastMessage?.data === "Player2") setPlayer("Player2")
    }, [lastMessage])

    useEffect(() => {
        toast.dismiss()
        toast("New status: " + player)
    }, [player])

    const handleDrag = (e: DraggableEvent, data: DraggableData) => {
        setCords({ x: data.x, y: data.y })
        if (flag) return

        flag = true
        setTimeout(() => (flag = false), DEBOUNCE_TIME)

        if (readyState === 1 && player !== null) {
            const msg = generateMessage(
                player,
                mapCords(data.x).toString()
            )
            console.log(msg)
            sendMessage(msg)
        }
    }

    return (
        <div className="App">
            <Draggable
                onDrag={handleDrag}
                position={cords}
                bounds={{
                    left: 0,
                    right: window.innerWidth - CONTROL_SIZE,
                    top: 0,
                    bottom: window.innerHeight - CONTROL_SIZE
                }}
            >
                <div className="control"></div>
            </Draggable>
            <ToastContainer position="bottom-right" theme="dark" />
        </div>
    )
}

export default App
