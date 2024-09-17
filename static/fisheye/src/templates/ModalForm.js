import { HandleForm } from "../FormController/HandleForm.js"

export class ModalForm {

    constructor(name) {
        this.name = name

        this.$wrapper = document.createElement("dialog")
        this.$wrapper.classList.add('contact_modal')
        this.$modal = document.createElement('div')
        this.$modal.classList.add('modal')
    }

    closeModal() {

        this.$modal.querySelector('.closeModal')
            .addEventListener('click', () => {
                this.$wrapper.remove()
            })

        this.$modal.querySelector('.closeModal')
            .addEventListener('keydown', (e) => {
                if (e.key === "Enter") {
                    this.$wrapper.remove()
                }
            })

        this.$wrapper.addEventListener('keydown', (e) => {
            if (e.key === "Escape") {
                this.$wrapper.remove()
            }
        })

    }

    onSubmit() {
        new HandleForm("contactForm", this.displayValidation.bind(this))
    }

    displayValidation() {
        this.$modal.innerHTML = `
            <div class="modal__validation">
                <p>Votre message à bien été envoyé a ${this.name}.</p>
            </div>
            <i class="fas fa-times closeModal" aria-label="Fermer la modale" tabindex="0"></i> 
        `
        this.closeModal()
        this.onMove()
    }

    onMove() {
        this.$wrapper.addEventListener('keydown', (e) => {

            const { value } = document.activeElement.classList

            if (value === "modal" && e.shiftKey && e.key === "Tab") {
                e.preventDefault()
                this.$wrapper.querySelector('.closeModal').focus()
            } else if (value.includes('closeModal') && e.key === "Tab" && !e.shiftKey) {
                e.preventDefault()
                this.$wrapper.querySelector('.modal').focus()
            }
        })
    }

    createModal() {
        const modal = `
            <div class="modal__header">
                <h1 tabindex="0">
                    Contactez-moi
                    <br>
                    ${this.name}
                </h1>
            </div>
            <form id="contactForm">
                <div class="formData">
                    <label for="firstName" tabindex="0">Prénom</label>
                    <input id="firstName" tabindex="0" name="firstName" type="text" aria-label="Entrez votre prénom." />
                </div>
                <div class="formData">
                    <label for="lastName" tabindex="0">Nom</label>
                    <input id="lastName" tabindex="0" name="lastName" type="text" aria-label="Entrez votre nom." />
                </div>
                <div class="formData">
                    <label tabindex="0" for"userEmail">Email</label>
                    <input tabindex="0" id="userEmail" name="userEmail" aria-label="Entrez votre adresse email." />
                </div>
                <div class="formData">
                    <label tabindex="0" for="messageContent">Votre message</label>
                    <textarea tabindex="0" name="messageContent" id="messageContent" cols="30" rows="10" aria-label="Entrez le message a destination de ${this.name}."></textarea>
                </div>
                <button type="submit" class="contact_button" >Envoyer</button>
            </form>
            <i class="fas fa-times closeModal" aria-label="Fermer la modale" tabindex="0"></i> 
        `
        this.$wrapper.setAttribute('aria-hidden', "false")
        this.$wrapper.setAttribute('role', "dialog")

        this.$modal.innerHTML = modal
        this.$modal.setAttribute('tabindex', "0")

        this.$wrapper.appendChild(this.$modal)
        document.body.appendChild(this.$wrapper)

        this.$wrapper.querySelector('.modal').focus()

        this.closeModal()
        this.onSubmit()
        this.onMove()
    }
}