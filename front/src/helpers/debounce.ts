export default function debounce(fn: Function, timeout: number) {
    let timer: NodeJS.Timeout | null = null

    return function (...rest: any[]) {
        if (timer === null) {
            console.log(timer)

            fn(...rest)
            timer = setTimeout(() => {
                timer = null
            }, timeout)
        }
    }
}
