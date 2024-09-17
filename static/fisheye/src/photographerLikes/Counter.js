export class PhotographerLikesCounter {

    constructor() {
        this._$likesCount = null
        this._count = 0
        this._data = []
    }

    getInitialCount(data) {
        this._data = data
        this._$likesCount = document.querySelector('.likes_count')

        this.update()
    }

    update() {
        this._count = this._data.reduce((accumulator, current) => accumulator += current.likes, 0)

        this._$likesCount.innerHTML = this._count
        this._$likesCount.setAttribute('aria-label', `Les photos et vidéos de ce photographe accumule ${this._count} j'aimes.`)

    }
}