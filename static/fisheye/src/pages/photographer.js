import { FetchPhotograph, FetchMedia } from "../api/FetchData.js";
import { MediaSorter } from "../templates/MediaSorter.js";
import LightboxModal from "../templates/LightboxModal.js";


class Photographer {

    constructor() {
        this.photographerId = new URLSearchParams(document.location.search).get('id')

        this.$mainWrapper = document.querySelector('main')

        this.$mediaWrapper = document.createElement('div')
        this.$mediaWrapper.classList.add('media__wrapper')


        this.photopraphersApi = new FetchPhotograph("./data/photographers.json")
        this.mediasApi = new FetchMedia("./data/photographers.json")

        this.lightboxModal = new LightboxModal()

    }

    async main() {

        const photographerData = await this.photopraphersApi.getOne(this.photographerId)
        const headerTemplate = photographerData.createHeader()
        const aboutPhotographer = photographerData.displayLikesAndPrices()
        document.body.appendChild(aboutPhotographer)

        const mediasData = await this.mediasApi.getAllByPhotographerId(this.photographerId, this.lightboxModal)

        photographerData.displayGalery(mediasData, this.$mediaWrapper)

        const mediaSorter = new MediaSorter(mediasData, this.lightboxModal, photographerData)

        this.$mainWrapper.appendChild(headerTemplate)
        mediaSorter.render()
        this.$mainWrapper.appendChild(this.$mediaWrapper)

    }
}

const app = new Photographer()
app.main()