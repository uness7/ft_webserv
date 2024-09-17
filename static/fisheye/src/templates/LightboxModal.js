export default class LightboxModal {

    constructor() {
        this.currentMedia = 0
    }

    nextMedia() {
        if (this.currentMedia === this.medias.length - 1) {
            this.currentMedia = 0
        } else {
            this.currentMedia++
        }

        this.updateMedia()
    }

    previousMedia() {
        if (this.currentMedia === 0) {
            this.currentMedia = this.medias.length - 1
        } else {
            this.currentMedia--
        }

        this.updateMedia()
    }

    updateMedia() {
        this.$wrapper.querySelector('.player__wrapper__media').innerHTML = `
        ${this.medias[this.currentMedia].image ?
                `<img tabindex="0" src="./assets/medias/${this.medias[this.currentMedia].photographerId}/${this.medias[this.currentMedia].image}" alt="${this.medias[this.currentMedia].title}" class="media__full" />` :
                `<video controls autoplay tabindex="0" src="./assets/medias/${this.medias[this.currentMedia].photographerId}/${this.medias[this.currentMedia].video}" alt="${this.medias[this.currentMedia].title}" class="media__full"></video>`}
            <p tabindex="0">${this.medias[this.currentMedia].title}</p>
        `
    }

    onMove() {
        this.$wrapper.addEventListener('keydown', (e) => {

            const { value } = document.activeElement.classList

            if (e.key === "ArrowLeft") {
                this.previousMedia()
            } else if (e.key === "ArrowRight") {
                this.nextMedia()
            } else if (value ===
                "player__wrapper" && e.shiftKey && e.key === "Tab") {
                e.preventDefault()
                this.$wrapper.querySelector(".close_btn").focus()
            } else if (value.includes('close_btn') && !e.shiftKey && e.key === "Tab") {
                e.preventDefault()
                this.$wrapper.querySelector(".player__wrapper").focus()
            }
        })

        this.$wrapper.querySelector('.arrow_next')
            .addEventListener("click", () => this.nextMedia())
        this.$wrapper.querySelector('.arrow_next')
            .addEventListener("keydown", (e) => {
                if (e.key === 'Enter') {
                    this.nextMedia()
                }
            })

        this.$wrapper.querySelector('.arrow_back')
            .addEventListener("click", () => this.previousMedia())
        this.$wrapper.querySelector('.arrow_back')
            .addEventListener("keydown", (e) => {
                if (e.key === 'Enter') {
                    this.previousMedia()
                }
            })

    }

    deleteModal() {
        const main = document.querySelector('main')

        this.$wrapper.remove()
        main.setAttribute('aria-hidden', "false")
    }

    onCloseModal() {
        const closeBtn = this.$wrapper.querySelector('.close_btn')

        closeBtn.addEventListener('click', this.deleteModal.bind(this))

        closeBtn.addEventListener('click', this.deleteModal.bind(this))

        closeBtn.addEventListener('keydown', (e) => {
            if (e.key === "Enter") {
                this.deleteModal()
            }
        })

        this.$wrapper.addEventListener('keydown', (e) => {
            if (e.key === "Escape") {
                this.deleteModal()
            }
        })

    }

    createLightbox() {
        this.$wrapper = document.createElement('dialog')
        this.$wrapper.classList.add('lightbox__modal')
        this.$wrapper.setAttribute('aria-hidden', "false")
        this.$wrapper.setAttribute('role', "dialog")

        const content = `
        <div class="player__wrapper" tabindex="0" aria-label="Vue rapprochée des médias">
            <div class="player__wrapper__media" >
                ${this.medias[this.currentMedia].image ?
                `<img tabindex="0" src="./assets/medias/${this.medias[this.currentMedia].photographerId}/${this.medias[this.currentMedia].image}" alt="${this.medias[this.currentMedia].title}" class="media__full" />` :
                `<video controls autoplay tabindex="0" src="./assets/medias/${this.medias[this.currentMedia].photographerId}/${this.medias[this.currentMedia].video}" alt="${this.medias[this.currentMedia].title}" class="media__full"></video>`}
                <p tabindex="0">${this.medias[this.currentMedia].title}</p>
            </div>
            <i class="fas fa-chevron-left arrow_back" tabindex="0" aria-label="Media précédent."></i>
            <i class="fas fa-chevron-right arrow_next" tabindex="0" aria-label="Media suivant."></i>
            <i class="fas fa-times close_btn" tabindex="0" aria-label="Fermer la modale."></i> 
        </div>`

        const { body } = document
        const main = document.querySelector('main')

        this.$wrapper.innerHTML = content
        body.appendChild(this.$wrapper)

        main.setAttribute('aria-hidden', "true")
        this.$wrapper.querySelector('.player__wrapper').focus()
        this.onCloseModal()
        this.onMove()

    }

    setMedias(medias) {
        this.medias = medias
    }

    render(currentMedia) {
        this.currentMedia = this.medias.findIndex((medias) => medias.id === currentMedia)
        this.createLightbox()
    }
}