function generateMessage(
    player: "Player1" | "Player2",
    value: string
) {
    let result = ""

    if (player == "Player1") result += 1
    if (player == "Player2") result += 2
    result += "/"
    result += value

    return result
}

export default generateMessage
