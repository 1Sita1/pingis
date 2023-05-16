function mapCords(cord: number) {
    const SCALE = 8
    const decimal = ((cord + 42) / window.innerWidth - 0.5) * SCALE
    const rounded = Math.round(decimal * 100) / 100

    return rounded
}

export default mapCords
