import { PhotographerModel } from "../models/PhotographerModel.js";
import { MediaModel } from "../models/MediaModel.js";
import { PhotographerLikesCounter } from "../photographerLikes/Counter.js";


class FetchData {
    constructor(url) {
        this._url = url
    }

    async get() {
        try {
            const data = await fetch(this._url)
            const response = await data.json()
            return response
        } catch (error) {
            console.error(error)
        }
    }

}

export class FetchPhotograph extends FetchData {
    constructor(url) {
        super(url)
    }

    async getAll() {
        const data = await this.get()
        return data.photographers.map(photographerData => new PhotographerModel(photographerData))
    }

    async getOne(id) {
        const allPhotograpers = await this.getAll()
        const photographer = allPhotograpers.find(person => person.id === parseInt(id))

        return new PhotographerModel(photographer)
    }
}

export class FetchMedia extends FetchData {
    constructor(url) {
        super(url)
        this._data = []

        this.likesCounter = new PhotographerLikesCounter()
    }

    async getAllByPhotographerId(id, lightboxModal) {
        const allMedias = await this.get()

        this._data = allMedias.media.filter(media => media.photographerId === parseInt(id)).sort((a, b) => new Date(a.date).getTime() - new Date(b.date).getTime())

        lightboxModal.setMedias(this._data)

        this.likesCounter.getInitialCount(this._data)

        return this._data.map(media => MediaModel.mediaFactory(media, this.likesCounter, lightboxModal))
    }
}
